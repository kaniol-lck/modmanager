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
    enum { NameColumn,
           IdColumn, AuthorsColumn = IdColumn,
           FileIdColumn, WebsiteColumn = FileIdColumn,
           RequiredColumn };
public:
    enum Mode { MODPACKS, MANIFEST_JSON, CF_HTML,  M_HTML };
    explicit ExportManifestJsonDialog(QWidget *parent, LocalModPath *modPath, Mode mode);
    ~ExportManifestJsonDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ExportManifestJsonDialog *ui;
    LocalModPath *modPath_;
    Mode mode_;
    QStandardItemModel model_;
    QList<LocalMod *> disabledMods_;
};

#endif // EXPORTMANIFESTJSONDIALOG_H
