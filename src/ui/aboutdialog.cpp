#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QUrl>
#include <QDesktopServices>
#include <QPainter>
#include <QWindow>
#include <QDebug>
#ifdef DE_KDE
#include <KWindowEffects>
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
    //TODO: window not blur in next show
    KWindowEffects::enableBlurBehind(QWindow::fromWinId(winId()));
    qDebug() << winId();
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
