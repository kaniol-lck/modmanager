#ifndef OPTIFINEMANAGER_H
#define OPTIFINEMANAGER_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

#include "exploremanager.h"
#include "optifineapi.h"
#include "bmclapi.h"

class OptifineMod;
class OptifineManager;
class OptifineManagerModel : public QAbstractListModel
{
    Q_OBJECT
    friend class OptifineManager;
public:
    OptifineManagerModel(OptifineManager *manager);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setItemHeight(int newItemHeight);

private:
    OptifineManager *manager_;
    int itemHeight_ = 100;
};

class OptifineManager : public ExploreManager
{
    Q_OBJECT
public:
    enum Column { ModColumn };
    explicit OptifineManager(QObject *parent = nullptr);

    void search();
    void searchMore();
    void refresh();

    const QList<OptifineMod *> &mods() const;

    OptifineManagerModel *model() const override;

private:
    OptifineAPI api_;
    BMCLAPI bmclapi_;
    OptifineManagerModel *model_;
    QList<OptifineMod *> mods_;
    std::unique_ptr<Reply<QList<OptifineModInfo> > > searchModsGetter_;

    void getModList() override;
};

#endif // OPTIFINEMANAGER_H
