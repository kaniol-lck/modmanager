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
    ModrinthMod *mod_;
    LocalModPath *downloadPath_ = nullptr;
    bool transltedSummary_ = false;

    void downloadFile(const ModrinthFileInfo &fileInfo);
};

#endif // MODRINTHMODITEMWIDGET_H
