#ifndef OPTIFINEMANAGER_H
#define OPTIFINEMANAGER_H

#include <QSortFilterProxyModel>

#include "exploremanager.h"
#include "optifineapi.h"
#include "bmclapi.h"
#include "modloadertype.h"

class OptifineMod;
class OptifineManager;
class OptifineManagerModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class OptifineManager;
public:
    enum Column { ModColumn, NameColumn/*, PatchColumn, TypeColumn*/, GameVersionColumn, FileNameColumn, PreviewColumn, MirrorLinkColumn };
    OptifineManagerModel(OptifineManager *manager);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
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

class OptifineManagerProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    OptifineManagerProxyModel(QObject *parent = nullptr);

    void setGameVersion(const GameVersion &newGameVersion);
    void setText(const QString &newStr);
    void setShowPreview(bool newShowPreview);

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &) const override;

private:
    GameVersion gameVersion_;
    QString text_;
    bool showPreview_;
};

#endif // OPTIFINEMANAGER_H
