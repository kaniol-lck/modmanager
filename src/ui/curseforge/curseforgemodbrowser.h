#ifndef CURSEFORGEMODBROWSER_H
#define CURSEFORGEMODBROWSER_H

#include "ui/explorebrowser.h"

class CurseforgeAPI;
class CurseforgeMod;
class LocalModPath;

namespace Ui {
class CurseforgeModBrowser;
}

class CurseforgeModBrowser : public ExploreBrowser
{
    Q_OBJECT

public:
    explicit CurseforgeModBrowser(QWidget *parent = nullptr);
    ~CurseforgeModBrowser();
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
    void on_modListWidget_doubleClicked(const QModelIndex &index);
    void on_sortSelect_currentIndexChanged(int);
    void on_loaderSelect_currentIndexChanged(int index);
    void on_downloadPathSelect_currentIndexChanged(int index);
    void on_openFolderButton_clicked();
private:
    Ui::CurseforgeModBrowser *ui;
    CurseforgeAPI *api_;
    QList<int> idList_;
    LocalModPath *downloadPath_ = nullptr;
    QString currentName_;
    int currentIndex_;
    int currentCategoryId_;
    GameVersion currentGameVersion_;
    bool isUiSet_ = false;
    bool hasMore_ = false;
    bool isSearching_ = false;
};

#endif // CURSEFORGEMODBROWSER_H
