#ifndef MODRINTHMODINFODIALOG_H
#define MODRINTHMODINFODIALOG_H

#include <QDialog>

class ModrinthMod;
class LocalMod;

namespace Ui {
class ModrinthModInfoDialog;
}

class ModrinthModInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModrinthModInfoDialog(QWidget *parent, ModrinthMod *mod, const QString &path, LocalMod *localMod = nullptr);
    ~ModrinthModInfoDialog();

signals:
    void downloadPathChanged(QString path);

public slots:
    void setDownloadPath(const QString &newDownloadPath);

private:
    Ui::ModrinthModInfoDialog *ui;
    ModrinthMod *mod_;
    LocalMod *localMod_ = nullptr;
    QString downloadPath_;
};

#endif // MODRINTHMODINFODIALOG_H
