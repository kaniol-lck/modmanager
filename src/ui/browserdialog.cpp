#include "browserdialog.h"
#include "ui_browserdialog.h"

#include "ui/browser.h"

#include <QKeyEvent>

BrowserDialog::BrowserDialog(QWidget *parent, Browser *browser) :
    QDialog(parent),
    ui(new Ui::BrowserDialog),
    browser_(browser)
{
    ui->setupUi(this);
    //as standalone dialog
    if(!parent) ui->buttonBox->hide();

    ui->hLayout->insertWidget(0, browser_);
    if(browser_->infoWidget()){
        ui->vLayout->insertWidget(0, browser_->infoWidget());
        browser_->infoWidget()->show();
    }
    ui->hLayout->setStretchFactor(0, 1);
    ui->hLayout->setStretchFactor(1, 3);
}

void BrowserDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        return;
    if(!parent() && event->key() == Qt::Key_Escape)
        return;
    QDialog::keyPressEvent(event);
}

BrowserDialog::~BrowserDialog()
{
    delete ui;
}
