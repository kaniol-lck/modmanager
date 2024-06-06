#ifndef CURSEFORGEFILELISTWIDGET_H
#define CURSEFORGEFILELISTWIDGET_H

#include <QStandardItemModel>
#include <QWidget>

#include "gameversion.h"
#include "modloadertype.h"

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
    void updateFileList2();
    void updateIndexWidget();
    void updateIndexWidget2();
    void onListSliderChanged(int i);
    void updateVersionList();
    void search(bool changed = false);

    void on_allFile_btn_toggled(bool checked);

    void on_loaderTypeSelecct_currentIndexChanged(int index);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::CurseforgeFileListWidget *ui;
    DownloadPathSelectMenu *downloadPathSelectMenu_ = nullptr;
    QStandardItemModel model_;
    QStandardItemModel model2_;
    CurseforgeMod *mod_ = nullptr;
    LocalMod *localMod_ = nullptr;
    QMenu *versionMenu_ = nullptr;
    GameVersion currentGameVersion_;
    ModLoaderType::Type currentLoaderType_;
};

#endif // CURSEFORGEFILELISTWIDGET_H
