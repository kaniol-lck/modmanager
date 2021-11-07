#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QUrl>
#include <QDesktopServices>
#include <QPainter>
#include <QWindow>
#ifdef DE_KDE
#include <KWindowEffects>
#endif
#ifdef Q_OS_WIN
#include "util/WindowCompositionAttribute.h"
#include <windowsx.h>
#include <dwmapi.h>
#endif

#include "version.h"
#include "windowstitlebar.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->versionText->setText(kVersion);
    setAttribute(Qt::WA_DeleteOnClose);
#ifdef DE_KDE
    KWindowEffects::enableBlurBehind(winId());
#endif
#ifdef Q_OS_WIN
//    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    if(auto huser = GetModuleHandle(L"user32.dll"); huser){
        auto setWindowCompositionAttribute = (pfnSetWindowCompositionAttribute)::GetProcAddress(huser, "SetWindowCompositionAttribute");
        if(setWindowCompositionAttribute){
            ACCENT_POLICY accent = { ACCENT_ENABLE_BLURBEHIND, 0x1e0, 0x000f0f0f, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data;
            data.Attrib = WCA_ACCENT_POLICY;
            data.pvData = &accent;
            data.cbData = sizeof(accent);
            setWindowCompositionAttribute(::HWND(winId()), &data);
        }
    }
    auto titleBar_ = new WindowsTitleBar(this, windowTitle());
    titleBar_->setIconVisible(false);
    ui->titleLayout->addWidget(titleBar_);
#endif
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::paintEvent(QPaintEvent *event[[maybe_unused]])
{
    if(!ui->leftWidget->isVisible()) return;
    QPainter p(this);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    auto rect = ui->leftWidget->rect();
    rect.translate(ui->leftWidget->pos());
    p.fillRect(rect, QBrush(QColor(255, 255, 255, 127)));
}

void AboutDialog::on_toolButton_clicked()
{
    QUrl url("https://github.com/kaniol-lck/modmanager");
    QDesktopServices::openUrl(url);
}

void AboutDialog::on_toolButton_2_clicked()
{
    QUrl url("https://github.com/kaniol-lck/modmanager/issues");
    QDesktopServices::openUrl(url);
}
