#include "curseforgestatusbarwidget.h"
#include "ui_curseforgestatusbarwidget.h"

CurseforgeStatusBarWidget::CurseforgeStatusBarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurseforgeStatusBarWidget)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
}

CurseforgeStatusBarWidget::~CurseforgeStatusBarWidget()
{
    delete ui;
}

void CurseforgeStatusBarWidget::setProgressVisible(bool visible)
{
    ui->progressBar->setVisible(visible);
}

void CurseforgeStatusBarWidget::setText(const QString &text)
{
    ui->label->setText(text);
}
