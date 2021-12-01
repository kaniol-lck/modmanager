#ifndef CURSEFORGEMODBROWSER_H
#define CURSEFORGEMODBROWSER_H

#include "ui/explorebrowser.h"

class CurseforgeAPI;
class CurseforgeMod;
class LocalModPath;
class CurseforgeModInfoWidget;
class CurseforgeFileListWidget;
class QListWidgetItem;

class QStandardItemModel;
class LocalMod;
namespace Ui {
class CurseforgeModBrowser;
}

class CurseforgeModBrowser : public ExploreBrowser
{
    Q_OBJECT

public:
    explicit CurseforgeModBrowser(QWidget *parent = nullptr, LocalMod *mod = nullptr);
    ~CurseforgeModBrowser();

    QWidget *infoWidget() const override;
    QWidget *fileListWidget() const override;
signals:
    void downloadPathChanged(LocalModPath *path);

public slots:
    void refresh() override;
    void searchModByPathInfo(const LocalModPathInfo &info) override;
    void updateUi() override;

private slots:
    void getModList(QString name, int index = 0, int needMore = 20);
    void updateVersionList();
    void updateCategoryList();
    void updateLocalPathList();
    void search();
    void onSliderChanged(int i);
    void on_modListView_doubleClicked(const QModelIndex &index);
    void on_loaderSelect_currentIndexChanged(int index);
    void on_downloadPathSelect_currentIndexChanged(int index);
    void on_openFolderButton_clicked();
    void onItemSelected(const QModelIndex &index);
    void updateIndexWidget();
    void on_sortSelect_currentIndexChanged(int);

protected:
    void paintEvent(QPaintEvent *event) override;
private:
    Ui::CurseforgeModBrowser *ui;
    QStandardItemModel *model_;
    CurseforgeModInfoWidget *infoWidget_;
    CurseforgeFileListWidget *fileListWidget_;
    CurseforgeAPI *api_;
    QList<int> idList_;
    LocalModPath *downloadPath_ = nullptr;
    //open as dialog to search mod
    LocalMod *localMod_;
    QString currentName_;
    int currentIndex_;
    int currentCategoryId_;
    GameVersion currentGameVersion_;
    ModLoaderType::Type currentLoaderType_ = ModLoaderType::Any;
    bool hasMore_ = false;
    bool isSearching_ = false;
    bool inited_ = false;
};

#endif // CURSEFORGEMODBROWSER_H
