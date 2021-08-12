#include "modmanager.h"
#include "ui_modmanager.h"

ModManager::ModManager(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ModManager)
{
    ui->setupUi(this);
}

ModManager::~ModManager()
{
    delete ui;
}

