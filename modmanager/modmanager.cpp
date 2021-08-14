#include "modmanager.h"
#include "ui_modmanager.h"

#include "quazip.h"
#include "quazipfile.h"
#include "MurmurHash2.h"

#include <QDebug>
#include <QDir>

#include "localmodbrowser.h"
#include "curseforgemodbrowser.h"

ModManager::ModManager(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ModManager)
{
    ui->setupUi(this);
    for(int i = ui->stackedWidget->count(); i >= 0; i--)
    {
        QWidget* widget = ui->stackedWidget->widget(i);
        ui->stackedWidget->removeWidget(widget);
        widget->deleteLater();
    }

    ModDirInfo modDirInfo(QDir("/run/media/kaniol/SanDisk/Minecraft/1.17.1/.minecraft/mods/"), "1.17.1");
    ModDirInfo modDirInfo2(QDir("/run/media/kaniol/SanDisk/Minecraft/1.16.5/.minecraft/mods/"), "1.16.5");

    modDirList.append(modDirInfo);
    modDirList.append(modDirInfo2);

    ui->modDirSelectorWidget->addItem("Explore");
    ui->stackedWidget->addWidget(new CurseforgeModBrowser(this));

    for(const auto &modDirInfo : qAsConst(modDirList)){
        if(modDirInfo.exists()) {
            ui->modDirSelectorWidget->addItem(modDirInfo.getGameVersion());
            ui->stackedWidget->addWidget(new LocalModBrowser(this, modDirInfo.getModDir()));
        }
    }

}

ModManager::~ModManager()
{
    delete ui;
}

void ModManager::on_modDirSelectorWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}

