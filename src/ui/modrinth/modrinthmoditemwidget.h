#ifndef MODRINTHMODITEMWIDGET_H
#define MODRINTHMODITEMWIDGET_H

#include <QWidget>

#include "modrinth/modrinthfileinfo.h"

class ModrinthMod;
class LocalModPath;

namespace Ui {
class ModrinthModItemWidget;
}

class ModrinthModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModrinthModItemWidget(QWidget *parent, ModrinthMod *mod);
    ~ModrinthModItemWidget();

    ModrinthMod *mod() const;

public slots:
    void setDownloadPath(LocalModPath *newDownloadPath);
    void updateUi();
    void downloadFile(const ModrinthFileInfo &fileInfo);

private slots:
    void updateIcon();
    void updateFileList();
    void on_modSummary_customContextMenuRequested(const QPoint &pos);

private:
    Ui::ModrinthModItemWidget *ui;
    ModrinthMod *mod_;
    LocalModPath *downloadPath_ = nullptr;
    bool transltedSummary_ = false;
};

#endif // MODRINTHMODITEMWIDGET_H
