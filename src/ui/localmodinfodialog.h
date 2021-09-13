#ifndef LOCALMODINFODIALOG_H
#define LOCALMODINFODIALOG_H

#include <QDialog>

class LocalMod;

namespace Ui {
class LocalModInfoDialog;
}

class LocalModInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LocalModInfoDialog(QWidget *parent, LocalMod *mod);
    ~LocalModInfoDialog();

public slots:
    void updateInfo();

private slots:
    void on_curseforgeButton_clicked();

    void on_modrinthButton_clicked();

    void on_websiteButton_clicked();

    void on_sourceButton_clicked();

    void on_issueButton_clicked();

private:
    Ui::LocalModInfoDialog *ui;
    LocalMod *mod_;
};

#endif // LOCALMODINFODIALOG_H
