#include "browserdialog.h"
#include "ui_browserdialog.h"

#include "ui/browser.h"

BrowserDialog::BrowserDialog(QWidget *parent, Browser *browser) :
    QDialog(parent),
    ui(new Ui::BrowserDialog),
    browser_(browser)
{
    ui->setupUi(this);
    ui->hSplitter->insertWidget(0, browser_);
    ui->vSplitter->insertWidget(0, browser_->infoWidget());
    browser_->infoWidget()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    browser_->infoWidget()->show();
}

BrowserDialog::~BrowserDialog()
{
    delete ui;
}
