#include "framelesswrapper.h"

#include <QApplication>
#include <QDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMdiArea>
#include <QPainter>
#include <QPushButton>
#include <QResizeEvent>
#include <QToolButton>
#include <QVBoxLayout>
#ifdef Q_OS_WIN
#include "util/WindowCompositionAttribute.h"
#include <windowsx.h>
#include <dwmapi.h>
#endif

#include "windowstitlebar.h"
#include "config.hpp"

FramelessWrapper::FramelessWrapper(QMainWindow *window) :
    FramelessWrapper(window, window->menuBar())
{}

FramelessWrapper::FramelessWrapper(QDialog *dialog) :
    FramelessWrapper((QWidget*)dialog)
{
    connect(dialog, &QDialog::finished, this, &FramelessWrapper::close);
}

FramelessWrapper::FramelessWrapper(QWidget *widget, QMenuBar *menuBar) :
    QMainWindow(widget->parentWidget()),
    titleBar_(new WindowsTitleBar(widget, menuBar))
{
    titleBar_->setParentWidget(this);
    setAttribute(Qt::WA_Hover);
    //关闭即销毁，否则每开一次偏好设置/关于/新浏览器窗口都会泄漏一个顶层窗口
    setAttribute(Qt::WA_DeleteOnClose);
    //注意：这里刻意不设置 WA_TranslucentBackground。
    //DWM 的 backdrop 模糊要求窗口自身带 alpha 通道，但非不透明窗口在
    //「跨不同 DPI 显示器拖动」和「缩小尺寸」时会出现黑块/未重绘区域
    //（Qt 在 DPI 变化或缩放时会重建 backing store，未覆盖处露出黑色），
    //代价远大于收益，故保持窗口不透明。

    auto w = new QWidget(this);
    auto layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(titleBar_);
    layout->addWidget(widget);
    w->setLayout(layout);
    setCentralWidget(w);

    updateBlur();
}

void FramelessWrapper::updateAllBlur()
{
    //遍历所有存活实例。不能只查 topLevelWidgets()——偏好/关于这类对话框的 wrapper 是带 parent 的。
    const auto widgets = QApplication::allWidgets();
    for(auto *widget : widgets)
        if(auto *frameless = qobject_cast<FramelessWrapper*>(widget))
            frameless->updateBlur();
}

void FramelessWrapper::updateBlur()
{
    //标题栏的半透明穿透是按 Config 决定的，要主动重绘才能看到开关变化
    titleBar_->update();

#ifdef Q_OS_WIN
    //user32 里的函数只解析一次
    static const auto setWindowCompositionAttribute = []() -> pfnSetWindowCompositionAttribute {
        if(auto huser = GetModuleHandle(L"user32.dll"); huser)
            return (pfnSetWindowCompositionAttribute)::GetProcAddress(huser, "SetWindowCompositionAttribute");
        return nullptr;
    }();
    if(!setWindowCompositionAttribute) return;

    ACCENT_STATE as = Config().getEnableBlurBehind() ? ACCENT_ENABLE_BLURBEHIND
                                                     : ACCENT_ENABLE_GRADIENT;
    ACCENT_POLICY accent = { as, 0x1e0, 0x000f0f0f, 0 };
    WINDOWCOMPOSITIONATTRIBDATA data;
    data.Attrib = WCA_ACCENT_POLICY;
    data.pvData = &accent;
    data.cbData = sizeof(accent);
    setWindowCompositionAttribute(::HWND(winId()), &data);
#endif //Q_OS_WIN
}

#ifdef Q_OS_WIN

#if QT_VERSION_MAJOR == 6
bool FramelessWrapper::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
#else
bool FramelessWrapper::nativeEvent(const QByteArray &eventType, void *message, long *result)
#endif  // QT_VERSION_MAJOR
{
    MSG* msg = (MSG*)message;
    //缩放热区（逻辑像素）
    const float boundaryWidth = 4;

    //support highdpi
    double dpr = this->devicePixelRatioF();

    switch(msg->message){
    case WM_NCCALCSIZE:{
        if(msg->wParam == FALSE) return false;
        *result = WVR_REDRAW;
        return true;
    }
    case WM_DPICHANGED:{
        //拖到另一块缩放比例不同的显示器时，Qt 会重建平台窗口，
        //DWM 的 accent（以及标题栏观感）会丢失，需要重新施加一次。
        //返回 false 把尺寸调整交回 Qt 处理。
        updateBlur();
        return false;
    }
    case WM_NCHITTEST:{
        //Windows 传入的 *result 初值不可依赖，必须显式置位，否则下面会读到垃圾值
        *result = HTNOWHERE;

        POINT nativeLocalPos{  GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
        ::ScreenToClient(msg->hwnd, &nativeLocalPos);
        auto mousePos =  QPoint(nativeLocalPos.x / dpr, nativeLocalPos.y / dpr);

        bool left = mousePos.x() < boundaryWidth;
        bool right = mousePos.x() > width() - boundaryWidth;
        bool top = mousePos.y() < boundaryWidth;
        bool bottom = mousePos.y() > height() - boundaryWidth;

        if(left && top)
            *result = HTTOPLEFT;
        else if(right && top)
            *result = HTTOPRIGHT;
        else if(left && bottom)
            *result = HTBOTTOMLEFT;
        else if(right && bottom)
            *result = HTBOTTOMRIGHT;
        else if(left)
            *result = HTLEFT;
        else if(right)
            *result = HTRIGHT;
        else if(top)
            *result = HTTOP;
        else if(bottom)
            *result = HTBOTTOM;

        //命中边框则交给系统处理缩放
        if(*result != HTNOWHERE) return true;

        //标题栏区域用自定义 hit test。坐标必须换算到标题栏自身坐标系：
        //mousePos 是 wrapper 客户区坐标，而 hitTest() 内部用 childAt()，要的是标题栏局部坐标。
        //两者现在恰好重合只是因为标题栏位于 (0,0)，一旦有内容边距就会整体偏移。
        const auto titleBarPos = titleBar_->mapFrom(this, mousePos);
        if(!titleBar_->rect().contains(titleBarPos)) return false;
        return titleBar_->hitTest(titleBarPos, result);
    }
    case WM_GETMINMAXINFO: {
        //先让系统填好默认值，再基于工作区修正。
        //原实现在 DefWindowProc 之前读 ptMaxPosition，拿到的还是未初始化值，等于没生效。
        ::DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);

        auto *minmaxInfo = reinterpret_cast<MINMAXINFO*>(msg->lParam);
        //按所在显示器的工作区（不含任务栏）限定最大化尺寸，
        //否则无边框窗口最大化会盖住任务栏。
        HMONITOR monitor = ::MonitorFromWindow(msg->hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo{};
        monitorInfo.cbSize = sizeof(MONITORINFO);
        if(monitor && ::GetMonitorInfo(monitor, &monitorInfo)){
            const RECT &work = monitorInfo.rcWork;
            const RECT &full = monitorInfo.rcMonitor;
            minmaxInfo->ptMaxPosition.x = work.left - full.left;
            minmaxInfo->ptMaxPosition.y = work.top - full.top;
            minmaxInfo->ptMaxSize.x = work.right - work.left;
            minmaxInfo->ptMaxSize.y = work.bottom - work.top;
            minmaxInfo->ptMaxTrackSize.x = minmaxInfo->ptMaxSize.x;
            minmaxInfo->ptMaxTrackSize.y = minmaxInfo->ptMaxSize.y;
        }

        //客户区就是整窗，不需要额外内容边距
        setContentsMargins(0, 0, 0, 0);
        if(::IsZoomed(msg->hwnd))
            titleBar_->setMaximumed();
        else
            titleBar_->setNormal();

        *result = 0;
        return true;
    }
    }
    return false;
}

#endif //Q_OS_WIN
