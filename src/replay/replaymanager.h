#ifndef REPLAYMANAGER_H
#define REPLAYMANAGER_H

#include "exploremanager.h"
#include "replay/replayapi.h"

class ReplayManager;
class ReplayManagerModel : public QAbstractListModel
{
    Q_OBJECT
    friend class ReplayManager;
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
public:
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

#endif // REPLAYMANAGER_H
