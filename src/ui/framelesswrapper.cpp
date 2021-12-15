#include "framelesswrapper.h"

#include <QMainWindow>
#include <QToolButton>
#include <QVBoxLayout>
#ifdef Q_OS_WIN
#include "util/WindowCompositionAttribute.h"
#include <windowsx.h>
#include <dwmapi.h>
#endif

#include "windowstitlebar.h"
#include "config.hpp"

FramelessWrapper::FramelessWrapper(QWidget *parent, QWidget *widget, QMenuBar *menuBar) :
    QWidget(parent)
{
    setLayout(new QVBoxLayout);
    layout()->setMargin(1);
    titleBar_ = new WindowsTitleBar(this, windowTitle(), menuBar);
    layout()->addWidget(titleBar_);
    layout()->addWidget(widget);

    //set blur
    if(auto huser = GetModuleHandle(L"user32.dll"); huser){
        auto setWindowCompositionAttribute = (pfnSetWindowCompositionAttribute)::GetProcAddress(huser, "SetWindowCompositionAttribute");
        if(setWindowCompositionAttribute){
            ACCENT_STATE as;
            if(Config().getEnableBlurBehind()){
//                    if(isMoving_)
                    as = ACCENT_ENABLE_BLURBEHIND;
//                    else
//                        as = ACCENT_ENABLE_ACRYLICBLURBEHIND;
            }
            else
                as = ACCENT_DISABLED;
            ACCENT_POLICY accent = { as, 0x1e0, 0x000f0f0f, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data;
            data.Attrib = WCA_ACCENT_POLICY;
            data.pvData = &accent;
            data.cbData = sizeof(accent);
            setWindowCompositionAttribute(::HWND(winId()), &data);
        }
    }
    HWND hwnd = (HWND)winId();
    DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);
    ::SetWindowLong(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CAPTION);
}

QWidget *FramelessWrapper::makeFrameless(QMainWindow *window)
{
    window->menuBar()->hide();
    auto wrapper = new FramelessWrapper(qobject_cast<QWidget *>(window->parent()),
                                      window,
                                      window->menuBar());
    return wrapper;
}

#ifdef Q_OS_WIN
bool FramelessWrapper::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* msg = (MSG*)message;
    int boundaryWidth = 10;
    switch(msg->message){
//    case WM_ENTERSIZEMOVE:{
//        if(!isMoving_){
//            isMoving_ = true;
//            updateBlur();
//        }
//        return true;
//    }
//    case WM_EXITSIZEMOVE:{
//        if(isMoving_){
//            isMoving_ = false;
//            updateBlur();
//        }
//        return true;
//    }
    case WM_NCCALCSIZE:{
        NCCALCSIZE_PARAMS& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
        if (params.rgrc[0].top != 0)
            params.rgrc[0].top -= 1;

        *result = WVR_REDRAW;
        return true;
    }
    case WM_NCHITTEST:{
        int xPos = GET_X_LPARAM(msg->lParam) - this->frameGeometry().x();
        int yPos = GET_Y_LPARAM(msg->lParam) - this->frameGeometry().y();
        if(xPos < boundaryWidth && yPos<boundaryWidth)
            *result = HTTOPLEFT;
        else if(xPos >= width()-boundaryWidth&&yPos<boundaryWidth)
            *result = HTTOPRIGHT;
        else if(xPos<boundaryWidth&&yPos >= height()-boundaryWidth)
            *result = HTBOTTOMLEFT;
        else if(xPos>=width()-boundaryWidth&&yPos>=height()-boundaryWidth)
            *result = HTBOTTOMRIGHT;
        else if(xPos < boundaryWidth)
            *result =  HTLEFT;
        else if(xPos>=width() - boundaryWidth)
            *result = HTRIGHT;
        else if(yPos < boundaryWidth)
            *result = HTTOP;
        else if(yPos >= height() - boundaryWidth)
            *result = HTBOTTOM;
        if(*result) return true;

        //support highdpi
        double dpr = this->devicePixelRatioF();
        long x = GET_X_LPARAM(msg->lParam);
        long y = GET_Y_LPARAM(msg->lParam);
        QPoint pos = titleBar_->mapFromGlobal(QPoint(x/dpr,y/dpr));

        if (!titleBar_->rect().contains(pos)) return false;
        QWidget* child = titleBar_->childAt(pos);
        if (!qobject_cast<QToolButton*>(child)){
            *result = HTCAPTION;
            return true;
        }
    }
    case WM_GETMINMAXINFO: {
        if (::IsZoomed(msg->hwnd)) {
            RECT frame = { 0, 0, 0, 0 };
            AdjustWindowRectEx(&frame, WS_OVERLAPPEDWINDOW, FALSE, 0);
            frame.left = abs(frame.left);
            frame.top = abs(frame.bottom);
            this->setContentsMargins(frame.left, frame.top, frame.right, frame.bottom);
        } else{
            this->setContentsMargins(0, 0, 0, 0);
        }
        *result = ::DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
         break;
    }
    }
    return false;
}
#endif //Q_OS_WIN
