#ifndef CURSEFORGEFILELISTWIDGET_H
#define CURSEFORGEFILELISTWIDGET_H

#include "ui/dockwidgetcontent.h"

class CurseforgeMod;
class LocalModPath;
class QStandardItemModel;
namespace Ui {
class CurseforgeFileListWidget;
}

class CurseforgeFileListWidget : public DockWidgetContent
{
    Q_OBJECT

public:
    explicit CurseforgeFileListWidget(QWidget *parent = nullptr);
    ~CurseforgeFileListWidget();

    void setMod(CurseforgeMod *mod);

signals:
    void modChanged();
    void downloadPathChanged(LocalModPath *path);

public slots:
    void setDownloadPath(LocalModPath *newDownloadPath);
    void updateUi();

private slots:
    void updateFileList();
    void updateIndexWidget();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::CurseforgeFileListWidget *ui;
    QStandardItemModel *model_;
    CurseforgeMod *mod_ = nullptr;
    LocalModPath *downloadPath_ = nullptr;
};

#endif // CURSEFORGEFILELISTWIDGET_H
