#ifndef EXPLOREBROWSER_H
#define EXPLOREBROWSER_H

#include "ui/browser.h"
#include "local/localmodpathinfo.h"
#include "explorestatusbarwidget.h"

#include <QListView>

class DownloadPathSelectMenu;
class QComboBox;
class LocalModPath;
class QStandardItem;
class QStandardItemModel;
class QMenu;
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
    void initUi();
    bool isRowHidden(int row);
    void setRowHidden(int row, bool hidden);
    void scrollToTop();
    virtual void loadMore();
    virtual void onSelectedItemChanged(QStandardItem *item);
    virtual QWidget *getIndexWidget(QStandardItem *item);
    virtual QDialog *getDialog(QStandardItem *item);
    virtual QMenu *getCustomContextMenu();
    ExploreStatusBarWidget *statusBarWidget_;
    QStandardItemModel *model_;
    QMenu *menu_;
    DownloadPathSelectMenu *downloadPathSelectMenu_;
    QAction *visitWebsiteAction_;
    QAction *refreshAction_;
    QAction *openDialogAction_;

private:
    QListView *modListView_;
    QIcon icon_;
    QString name_;
};

#endif // EXPLOREBROWSER_H
