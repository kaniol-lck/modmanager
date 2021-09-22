#ifndef LOCALMODINFODIALOG_H
#define LOCALMODINFODIALOG_H

#include <QDialog>

class LocalMod;
class LocalModFile;

namespace Ui {
class LocalModInfoDialog;
}

class LocalModInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LocalModInfoDialog(QWidget *parent, LocalModFile *file, LocalMod *mod = nullptr);

    ~LocalModInfoDialog();

public slots:
    void updateInfo();

private slots:
    void on_curseforgeButton_clicked();

    void on_modrinthButton_clicked();

    void on_homepageButton_clicked();

    void on_sourceButton_clicked();

    void on_issueButton_clicked();

    void on_oldModListWidget_doubleClicked(const QModelIndex &index);

    void on_disableButton_toggled(bool checked);

    void on_editAliasButton_toggled(bool checked);

    void on_editFileNameButton_toggled(bool checked);

private:
    Ui::LocalModInfoDialog *ui;
    LocalModFile *file_;
    LocalMod *mod_ = nullptr;

    bool isRenaming = false;
};

#endif // LOCALMODINFODIALOG_H
