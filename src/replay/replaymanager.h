#ifndef REPLAYMANAGER_H
#define REPLAYMANAGER_H

#include "exploremanager.h"
#include "replay/replayapi.h"

#include <QSortFilterProxyModel>

class ReplayManager;
class ReplayManagerModel : public QAbstractListModel
{
    Q_OBJECT
    friend class ReplayManager;
public:
    ReplayManagerModel(ReplayManager *manager);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setItemHeight(int newItemHeight);

private:
    ReplayManager *manager_;
    int itemHeight_ = 100;
};

class ReplayManager : public ExploreManager
{
    Q_OBJECT
public:
    enum Column { ModColumn };
    explicit ReplayManager(QObject *parent = nullptr);

    void search();
    ReplayManagerModel *model() const override;

    const QList<ReplayMod *> &mods() const;

private:
    ReplayAPI api_;
    ReplayManagerModel *model_;
    QList<ReplayMod *> mods_;
    std::unique_ptr<Reply<QList<ReplayModInfo> > > searchModsGetter_;

    void getModList() override;
};

class ReplayManagerProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    ReplayManagerProxyModel(QObject *parent = nullptr);

    void setGameVersion(const GameVersion &newGameVersion);
    void setLoaderType(ModLoaderType::Type newLoaderType);
    void setText(const QString &newText);

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &) const override;

private:
    GameVersion gameVersion_;
    ModLoaderType::Type loaderType_;
    QString text_;
};

#endif // REPLAYMANAGER_H
