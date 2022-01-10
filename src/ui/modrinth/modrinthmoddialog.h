#ifndef MODRINTHMODDIALOG_H
#define MODRINTHMODDIALOG_H

#include <QDialog>

class ModrinthMod;
class ModrinthModBrowser;
class LocalMod;
class LocalModPath;
namespace Ui {
class ModrinthModDialog;
}

class ModrinthModDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModrinthModDialog(ModrinthModBrowser *parent, ModrinthMod *mod);
    explicit ModrinthModDialog(QWidget *parent, ModrinthMod *mod, LocalMod *localMod = nullptr);
    ~ModrinthModDialog();

private slots:
    void updateBasicInfo();
    void updateFullInfo();
    void updateIcon();
    void on_websiteButton_clicked();
    void on_modSummary_customContextMenuRequested(const QPoint &pos);

private:
    Ui::ModrinthModDialog *ui;
    ModrinthMod *mod_;
    LocalMod *localMod_ = nullptr;
    bool transltedSummary_ = false;
};

#endif // MODRINTHMODDIALOG_H
