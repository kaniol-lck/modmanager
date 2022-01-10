#ifndef MODRINTHFILEITEMWIDGET_H
#define MODRINTHFILEITEMWIDGET_H

#include <QWidget>

#include "modrinth/modrinthfileinfo.h"

class ModrinthMod;
class LocalMod;
class LocalModPath;
class ModrinthFileListWidget;
namespace Ui {
class ModrinthFileItemWidget;
}

class ModrinthFileItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModrinthFileItemWidget(ModrinthFileListWidget *parent, ModrinthMod *mod, const ModrinthFileInfo &info, LocalMod* localMod = nullptr);
    ~ModrinthFileItemWidget();

public slots:
    void onDownloadPathChanged();

private slots:
    void updateLocalInfo();

    void on_downloadButton_clicked();

    void on_ModrinthFileItemWidget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::ModrinthFileItemWidget *ui;
    ModrinthFileListWidget *fileList_ = nullptr;
    ModrinthMod *mod_;
    LocalMod *localMod_ = nullptr;
    ModrinthFileInfo fileInfo_;
};

#endif // MODRINTHFILEITEMWIDGET_H
