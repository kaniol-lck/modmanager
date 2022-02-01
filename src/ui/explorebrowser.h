#ifndef EXPLOREBROWSER_H
#define EXPLOREBROWSER_H

#include "ui/browser.h"
#include "local/localmodpathinfo.h"
#include "explorestatusbarwidget.h"

#include <QListView>

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
    void onSliderChanged(int i);
    void onItemSelected();
    void updateIndexWidget();
    void onCustomContextMenuRequested(const QPoint &pos);
    void onDoubleClicked(const QModelIndex &index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void initUi(ExploreManager *manager, QAbstractItemModel *model);
    void initUi(ExploreManager *manager);
    void initUi(ExploreManager *manager, QSortFilterProxyModel *model);
    bool isRowHidden(int row);
    void setRowHidden(int row, bool hidden);
    void scrollToTop();
    virtual void loadMore();
    virtual void onSelectedItemChanged(const QModelIndex &index);
    virtual QWidget *getIndexWidget(const QModelIndex &index);
    virtual QDialog *getDialog(const QModelIndex &index);
    virtual QMenu *getCustomContextMenu();
    ExploreStatusBarWidget *statusBarWidget_;
    QMenu *menu_;
    DownloadPathSelectMenu *downloadPathSelectMenu_;
    QAction *visitWebsiteAction_;
    QAction *refreshAction_;
    QAction *openDialogAction_;

private:
    QAbstractItemModel *model() const;
    QListView *modListView_;
    QIcon icon_;
    QString name_;

    virtual void updateStatusText() = 0;
};

#endif // EXPLOREBROWSER_H
