#include "localstatusbarwidget.h"
#include "ui_localstatusbarwidget.h"

#include <QButtonGroup>
#include <QDebug>

LocalStatusBarWidget::LocalStatusBarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LocalModStatusBar),
    group_(new QButtonGroup(this))
{
    ui->setupUi(this);
    group_->addButton(ui->listButton, ListMode);
    group_->addButton(ui->iconButton, IconMode);
    group_->addButton(ui->detailButton, DetailMode);
    connect(group_, &QButtonGroup::idClicked, this, &LocalStatusBarWidget::viewModeChanged);
    ui->listButton->setChecked(true);
}

LocalStatusBarWidget::~LocalStatusBarWidget()
{
    delete ui;
}

QLabel *LocalStatusBarWidget::label() const
{
    return ui->label;
}

QProgressBar *LocalStatusBarWidget::progressBar() const
{
    return ui->progressBar;
}
