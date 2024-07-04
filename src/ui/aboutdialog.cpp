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
    ui->credit_label->setText(QString("Built with Qt %1\n"
                                      "3rd-Party Libraries:\n"
                                      "libaria2   https://aria2.github.io/\n"
                                      "Quazip     https://github.com/stachenov/quazip\n"
                                      "cpp-semver https://github.com/easz/cpp-semver\n"
                                      "toml++     https://github.com/marzer/tomlplusplus")
                                  .arg(QT_VERSION_STR));

    setAttribute(Qt::WA_DeleteOnClose);
#ifdef DE_KDE
    //TODO: window not blur in next show
    if(auto window = QWindow::fromWinId(winId()))
        KWindowEffects::enableBlurBehind(window);
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
