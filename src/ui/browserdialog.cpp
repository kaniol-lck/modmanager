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
    ui->hLayout->insertWidget(0, browser_);
    ui->vLayout->insertWidget(0, browser_->infoWidget());
    browser_->infoWidget()->show();
    ui->hLayout->setStretchFactor(0, 1);
    ui->hLayout->setStretchFactor(1, 3);
}

void BrowserDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        return;
    QDialog::keyPressEvent(event);
}

BrowserDialog::~BrowserDialog()
{
    delete ui;
}
