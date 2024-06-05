#ifndef CURSEFORGEMANAGER_H
#define CURSEFORGEMANAGER_H

#include <QSortFilterProxyModel>

#include "exploremanager.h"
#include "curseforgeapi.h"

class CurseforgeMod;
class CurseforgeManager;
class CurseforgeManagerModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class CurseforgeManager;
public:
    enum Column { ModColumn, NameColumn, ProjectIDColumn, SlugColumn, AuthorsColumn, CategoryColumn, WebsiteColumn, UpdateDateColumn, CreateDateColumn, ReleaseDateColumn, DownloadCountColumn, PopularityRankColumn, SummaryColumn };
    CurseforgeManagerModel(CurseforgeManager *manager);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setItemHeight(int newItemHeight);

private:
    CurseforgeManager *manager_;
    int itemHeight_ = 100;
};

class CurseforgeManager : public ExploreManager
{
    Q_OBJECT
public:
    CurseforgeManager(QObject *parent = nullptr, CurseforgeAPI::Section sectionId = CurseforgeAPI::Mod);

    void search(const QString &name, int categoryId, GameVersion version, int sort);
    void searchMore();
    void refresh();

    CurseforgeManagerModel *model() const override;
    const QList<CurseforgeMod *> &mods() const;

    void setSectionId(CurseforgeAPI::Section newSectionId);

private:
    CurseforgeAPI api_;
    CurseforgeManagerModel *model_;
    QList<CurseforgeMod *> mods_;
    CurseforgeAPI::Section sectionId_;
    QString currentName_;
    int currentIndex_;
    int currentCategoryId_ = 0;
    int currentSort_ = 0;
    GameVersion currentGameVersion_;
    ModLoaderType::Type currentLoaderType_ = ModLoaderType::Any;
    bool hasMore_ = false;
    std::unique_ptr<Reply<QList<CurseforgeModInfo> > > searchModsGetter_;

    void getModList() override;
};

#endif // CURSEFORGEMANAGER_H
