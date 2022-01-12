#ifndef ADDDOWNLOADDIALOG_H
#define ADDDOWNLOADDIALOG_H

#include <QDialog>

namespace Ui {
class AddDownloadDialog;
}

class AddDownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDownloadDialog(QWidget *parent = nullptr);
    ~AddDownloadDialog();

private slots:
    void on_selectDownloadPathButton_clicked();
    void on_AddDownloadDialog_accepted();
    void on_downloadUrl_textChanged(const QString &arg1 = QString());

    void on_pasteButton_clicked();

private:
    Ui::AddDownloadDialog *ui;
};

#endif // ADDDOWNLOADDIALOG_H
