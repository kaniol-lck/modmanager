#include "modrinthfileitemwidget.h"
#include "ui_modrinthfileitemwidget.h"

#include "util/funcutil.h"

ModrinthFileItemWidget::ModrinthFileItemWidget(QWidget *parent, const ModrinthFileInfo &info) :
    QWidget(parent),
    ui(new Ui::ModrinthFileItemWidget),
    modrinthFileInfo(info)
{
    ui->setupUi(this);
    ui->displayNameText->setText(info.getDisplayName());
    ui->downloadProgress->setVisible(false);
    //file name and link
    QString linkText = info.getFileName();
    linkText = "<a href=%1>" + linkText + "</a>";
    ui->fileNameText->setText(linkText.arg(info.getUrl().toString()));

    //game version
    QString gameversionText;
    for(const auto &ver : info.getGameVersions())
        gameversionText.append(ver).append(" ");
    ui->gameVersionText->setText(gameversionText);

    //loader type
    QString loaderTypeText;
    for(const auto &loader : info.getModLoaders())
        loaderTypeText.append(ModLoaderType::toString(loader)).append(" ");
    ui->loaderTypeText->setText(loaderTypeText);

    //size
    ui->downloadSpeedText->setVisible(false);

    //set timer
//    speedTimer.setInterval(1000 / 4);
//    connect(&speedTimer, &QTimer::timeout, this, &ModrinthFileItemWidget::updateDownlaodSpeed);
}

ModrinthFileItemWidget::~ModrinthFileItemWidget()
{
    delete ui;
}
