#ifndef LOCALMODDIALOG_H
#define LOCALMODDIALOG_H

#include <QDialog>

class LocalMod;
class LocalModFile;
class FlowLayout;

namespace Ui {
class LocalModDialog;
}

class LocalModDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LocalModDialog(QWidget *parent, LocalMod *mod = nullptr);

    ~LocalModDialog();

public slots:
    void onCurrentModChanged();
    void onCurrentFileChanged();

private slots:
    void on_curseforgeButton_clicked();

    void on_modrinthButton_clicked();

    void on_homepageButton_clicked();

    void on_sourceButton_clicked();

    void on_issueButton_clicked();

    void on_disableButton_toggled(bool checked);

    void on_editAliasButton_toggled(bool checked);

    void on_editFileNameButton_toggled(bool checked);

    void on_versionSelect_currentIndexChanged(int index);

    void on_rollbackButton_clicked();

    void on_deleteButton_clicked();

    void on_tagsWidget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::LocalModDialog *ui;
    FlowLayout *tagsLayout_;
    QList<QWidget*> tagWidgets_;
    LocalModFile *file_;
    LocalMod *mod_ = nullptr;

    bool isRenaming = false;
};

#endif // LOCALMODDIALOG_H
