#ifndef MODRINTHMODBROWSER_H
#define MODRINTHMODBROWSER_H

#include "ui/explorebrowser.h"

class ModrinthAPI;
class ModrinthMod;
class LocalModPath;
class ModrinthModInfoWidget;

class QListWidgetItem;
namespace Ui {
class ModrinthModBrowser;
}

class ModrinthModBrowser : public ExploreBrowser
{
    Q_OBJECT

public:
    explicit ModrinthModBrowser(QWidget *parent = nullptr);
    ~ModrinthModBrowser();

    QWidget *infoWidget() const override;
signals:
    void downloadPathChanged(LocalModPath *path);

public slots:
    void refresh() override;
    void searchModByPathInfo(const LocalModPathInfo &info) override;
    void updateUi() override;

private slots:
    void updateVersionList();
    void updateCategoryList();
    void updateLocalPathList();
    void search();
    void onSliderChanged(int i);
    void on_modListWidget_doubleClicked(const QModelIndex &index);
    void on_sortSelect_currentIndexChanged(int);
    void on_versionSelect_currentIndexChanged(int);
    void on_loaderSelect_currentIndexChanged(int);
    void on_openFolderButton_clicked();
    void on_downloadPathSelect_currentIndexChanged(int index);
    void on_categorySelect_currentIndexChanged(int);
    void on_modListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
private:
    Ui::ModrinthModBrowser *ui;
    ModrinthModInfoWidget *infoWidget_;
    ModrinthAPI *api_;
    LocalModPath *downloadPath_ = nullptr;
    QString currentName_;
    int currentIndex_;
    QStringList lastCategoryIds_;
    QStringList currentCategoryIds_;
    QList<GameVersion> lastGameVersions_;
    QList<GameVersion> currentGameVersions_;
    bool isUiSet_ = false;
    bool hasMore_ = false;
    bool isSearching_ = false;

    void getModList(QString name, int index = 0);
};

#endif // MODRINTHMODBROWSER_H
