#include "explorestatusbarwidget.h"
#include "ui_explorestatusbarwidget.h"

#include <QButtonGroup>

ExploreStatusBarWidget::ExploreStatusBarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExploreStatusBarWidget),
    group_(new QButtonGroup(this))
{
    ui->setupUi(this);
    group_->addButton(ui->listButton, ListMode);
    group_->addButton(ui->iconButton, IconMode);
    group_->addButton(ui->detailButton, DetailMode);
    connect(group_, &QButtonGroup::idClicked, this, &ExploreStatusBarWidget::viewModeChanged);
    ui->listButton->setChecked(true);
    ui->progressBar->setVisible(false);
}

ExploreStatusBarWidget::~ExploreStatusBarWidget()
{
    delete ui;
}

void ExploreStatusBarWidget::setProgressVisible(bool visible)
{
    ui->progressBar->setVisible(visible);
}

void ExploreStatusBarWidget::setText(const QString &text)
{
    ui->label->setText(text);
}

void ExploreStatusBarWidget::setModCount(int shownCount, int loadCount)
{
    ui->shownCount->setText(tr("Shown: %1").arg(shownCount));
    ui->loadCount->setText(tr("Loaded: %1").arg(loadCount));
}
