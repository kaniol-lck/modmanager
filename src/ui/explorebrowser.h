#ifndef EXPLOREBROWSER_H
#define EXPLOREBROWSER_H

#include "ui/browser.h"
#include "local/localmodpathinfo.h"
#include "explorestatusbarwidget.h"

#include <QListView>

class QStackedWidget;
class QTreeView;
class ExploreManager;
class DownloadPathSelectMenu;
class QComboBox;
class LocalModPath;
class QStandardItem;
class QStandardItemModel;
class QMenu;
class QSortFilterProxyModel;
class ExploreBrowser : public Browser
{
    Q_OBJECT
public:
    explicit ExploreBrowser(QWidget *parent, const QIcon &icon, const QString &name, const QUrl &url);
    virtual ~ExploreBrowser() = default;

    virtual ExploreBrowser *another() = 0;

    QAction *visitWebsiteAction() const;
    QIcon icon() const override;
    QString name() const override;

    LocalModPath *downloadPath() const;
    DownloadPathSelectMenu *downloadPathSelectMenu() const;

    QMenu *menu() const;

public slots:
    virtual void refresh() = 0;
    virtual void searchModByPathInfo(LocalModPath *path) = 0;

private slots:
    void onListSliderChanged(int i);
    void onIconListSliderChanged(int i);
    void onTreeSliderChanged(int i);
    void onItemSelected();
    void updateIndexWidget();
    void onCustomContextMenuRequested(const QPoint &pos);
    void onDoubleClicked(const QModelIndex &index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void initUi(ExploreManager *manager, QAbstractItemModel *model);
    void initUi(ExploreManager *manager);
    void initUi(ExploreManager *manager, QSortFilterProxyModel *proxyModel);
    bool isRowHidden(int row);
    void setRowHidden(int row, bool hidden);
    void scrollToTop();
    void updateStatusText();
    virtual void loadMore();
    virtual void onSelectedItemChanged(const QModelIndex &index);
    virtual QWidget *getIndexWidget(const QModelIndex &index);
    virtual QDialog *getDialog(const QModelIndex &index);
    virtual QMenu *getCustomContextMenu();
    ExploreManager *manager_ = nullptr;
    QSortFilterProxyModel *proxyModel_ = nullptr;
    QStatusBar *statusBar_;
    ExploreStatusBarWidget *statusBarWidget_;
    QMenu *menu_;
    DownloadPathSelectMenu *downloadPathSelectMenu_;
    QAction *visitWebsiteAction_;
    QAction *refreshAction_;
    QAction *openDialogAction_;

private:
    QAbstractItemModel *model() const;
    QStackedWidget *stackedWidget_;
    QListView *modListView_;
    QListView *modIconListView_;
    QTreeView *modTreeView_;
    QIcon icon_;
    QString name_;
};

#endif // EXPLOREBROWSER_H
