#ifndef PAGESWITCHER_H
#define PAGESWITCHER_H

#include <QStackedWidget>

class ExploreBrowser;
class DownloadBrowser;
class CurseforgeModBrowser;
class ModrinthModBrowser;
class OptifineModBrowser;
class ReplayModBrowser;
class LocalModBrowser;
class LocalModPath;

class PageSwitcher : public QStackedWidget
{
    Q_OBJECT
public:
    enum BrowserCategory{ Download, Explore, Local };
    explicit PageSwitcher(QWidget *parent = nullptr);

    void addDownloadPage();
    void addCurseforgePage();
    void addModrinthPage();
    void addOptiFinePage();
    void addReplayModPage();
    void addLocalPage(LocalModBrowser *browser);
    void addLocalPage(LocalModPath *path);

    LocalModBrowser *takeLocalModBrowser(int index);
    void removeLocalModBrowser(int index);

    DownloadBrowser *downloadBrowser() const;
    CurseforgeModBrowser *curseforgeModBrowser() const;
    ModrinthModBrowser *modrinthModBrowser() const;
    OptifineModBrowser *optifineModBrowser() const;
    ReplayModBrowser *replayModBrowser() const;
    const QList<ExploreBrowser *> &exploreBrowsers() const;
    const QList<LocalModBrowser *> &localModBrowsers() const;
    ExploreBrowser *exploreBrowser(int index) const;
    LocalModBrowser *localModBrowser(int index) const;
public slots:
    void setPage(int category, int page);
private:
    QVector<int> pageCount_;

    DownloadBrowser *downloadBrowser_;
    CurseforgeModBrowser *curseforgeModBrowser_;
    ModrinthModBrowser *modrinthModBrowser_;
    OptifineModBrowser *optifineModBrowser_;
    ReplayModBrowser *replayModBrowser_;
    QList<ExploreBrowser *> exploreBrowsers_;
    QList<LocalModBrowser *> localModBrowsers_;
};

#endif // PAGESWITCHER_H
