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

    QList<QAction *> modActions() const override;
    QList<QAction *> pathActions() const override;

    QAction *visitWebsiteAction() const;
    QIcon icon() const override;
    QString name() const override;

    LocalModPath *downloadPath() const;

public slots:
    virtual void refresh() = 0;
    virtual void searchModByPathInfo(LocalModPath *path) = 0;

    void openDialog();

signals:
    void downloadPathChanged(LocalModPath *path);

private slots:
    void onSliderChanged(int i);
    void onItemSelected();
    void updateIndexWidget();
    void onCustomContextMenuRequested(const QPoint &pos);
    void onDoubleClicked(const QModelIndex &index);
    void updateLocalPathList();
    void setDownloadPathIndex(int i);

protected:
    void paintEvent(QPaintEvent *event) override;
    void initUi();
    bool isRowHidden(int row);
    void setRowHidden(int row, bool hidden);
    virtual void loadMore();
    virtual void onSelectedItemChanged(QStandardItem *item);
    virtual QWidget *getIndexWidget(QStandardItem *item);
    virtual QDialog *getDialog(QStandardItem *item);
    virtual QMenu *getMenu();
    ExploreStatusBarWidget *statusBarWidget_;
    QStandardItemModel *model_;
    QMenu *modMenu_;
    QMenu *pathMenu_;
    DownloadPathSelectMenu *downloadPathSelectMenu_;
    //mod download dest
//    LocalModPath *downloadPath_ = nullptr;

private:
    QListView *modListView_;
    QIcon icon_;
    QString name_;
    QAction *visitWebsiteAction_;
};

#endif // EXPLOREBROWSER_H
