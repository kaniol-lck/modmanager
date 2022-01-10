#ifndef MODRINTHMODITEMWIDGET_H
#define MODRINTHMODITEMWIDGET_H

#include <QWidget>

#include "modrinth/modrinthfileinfo.h"

class ModrinthMod;
class ModrinthModBrowser;
class LocalModPath;

namespace Ui {
class ModrinthModItemWidget;
}

class ModrinthModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModrinthModItemWidget(ModrinthModBrowser *parent, ModrinthMod *mod);
    ~ModrinthModItemWidget();

    ModrinthMod *mod() const;

public slots:
    void onDownloadPathChanged();
    void updateUi();

private slots:
    void updateIcon();
    void updateFileList();
    void onDownloadStarted();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadSpeed(qint64 bytesPerSec);
    void onDownloadFinished();
    void on_modSummary_customContextMenuRequested(const QPoint &pos);

private:
    Ui::ModrinthModItemWidget *ui;
    ModrinthModBrowser *browser_ = nullptr;
    ModrinthMod *mod_;
    bool transltedSummary_ = false;

    void downloadFile(const ModrinthFileInfo &fileInfo);
};

#endif // MODRINTHMODITEMWIDGET_H
