#ifndef CURSEFORGEFILELISTWIDGET_H
#define CURSEFORGEFILELISTWIDGET_H

#include <QStandardItemModel>
#include <QWidget>

#include "gameversion.h"

class CurseforgeMod;
class CurseforgeModBrowser;
class LocalModPath;
class DownloadPathSelectMenu;
class LocalMod;

class QMenu;
namespace Ui {
class CurseforgeFileListWidget;
}

class CurseforgeFileListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeFileListWidget(CurseforgeModBrowser *parent);
    explicit CurseforgeFileListWidget(QWidget *parent = nullptr, LocalMod *localMod = nullptr);
    ~CurseforgeFileListWidget();

    void setMod(CurseforgeMod *mod);
    DownloadPathSelectMenu *downloadPathSelectMenu() const;

    void setLocalMod(LocalMod *newLocalMod);
    void setDownloadPathSelectMenu(DownloadPathSelectMenu *newDownloadPathSelectMenu);

signals:
    void modChanged();

public slots:
    void updateUi();

private slots:
    void updateFileList();
    void updateIndexWidget();
    void onListSliderChanged(int i);
    void updateVersionList();
    void search(bool changed = false);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::CurseforgeFileListWidget *ui;
    DownloadPathSelectMenu *downloadPathSelectMenu_ = nullptr;
    QStandardItemModel model_;
    CurseforgeMod *mod_ = nullptr;
    LocalMod *localMod_ = nullptr;
    QMenu *versionMenu_ = nullptr;
    GameVersion currentGameVersion_;
};

#endif // CURSEFORGEFILELISTWIDGET_H
