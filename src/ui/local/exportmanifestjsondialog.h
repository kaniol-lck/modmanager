#ifndef EXPORTMANIFESTJSONDIALOG_H
#define EXPORTMANIFESTJSONDIALOG_H

#include <QDialog>
#include <QStandardItem>

class LocalModPath;
class LocalMod;
namespace Ui {
class ExportManifestJsonDialog;
}

class ExportManifestJsonDialog : public QDialog
{
    Q_OBJECT
    enum { NameColumn, IdColumn, FileIdColumn, RequiredColumn };
public:
    explicit ExportManifestJsonDialog(QWidget *parent, LocalModPath *modPath);
    ~ExportManifestJsonDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ExportManifestJsonDialog *ui;
    LocalModPath *modPath_;
    QStandardItemModel model_;
    QList<LocalMod *> disabledMods_;
};

#endif // EXPORTMANIFESTJSONDIALOG_H
