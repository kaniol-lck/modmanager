#ifndef BROWSERDIALOG_H
#define BROWSERDIALOG_H

#include <QDialog>

class Browser;
namespace Ui {
class BrowserDialog;
}

class BrowserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BrowserDialog(QWidget *parent, Browser *browser);
    ~BrowserDialog();

private:
    Ui::BrowserDialog *ui;
    Browser *browser_;
};

#endif // BROWSERDIALOG_H
