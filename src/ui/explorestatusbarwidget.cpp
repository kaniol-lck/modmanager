#include "explorestatusbarwidget.h"
#include "ui_explorestatusbarwidget.h"

ExploreStatusBarWidget::ExploreStatusBarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExploreStatusBarWidget)
{
    ui->setupUi(this);
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
