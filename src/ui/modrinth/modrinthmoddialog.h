#ifndef MODRINTHMODDIALOG_H
#define MODRINTHMODDIALOG_H

#include <QDialog>

class ModrinthMod;
class LocalMod;
class LocalModPath;

namespace Ui {
class ModrinthModDialog;
}

class ModrinthModDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModrinthModDialog(QWidget *parent, ModrinthMod *mod, LocalMod *localMod = nullptr);
    ~ModrinthModDialog();

signals:
    void downloadPathChanged(LocalModPath *path);

public slots:
    void setDownloadPath(LocalModPath *newDownloadPath);

private slots:
    void updateBasicInfo();
    void updateFullInfo();
    void updateFileList();
    void updateIcon();
    void on_websiteButton_clicked();

private:
    Ui::ModrinthModDialog *ui;
    ModrinthMod *mod_;
    LocalMod *localMod_ = nullptr;
    LocalModPath *downloadPath_ = nullptr;
};

#endif // MODRINTHMODDIALOG_H
