#ifndef MODRINTHMANAGER_H
#define MODRINTHMANAGER_H

#include <QAbstractListModel>

#include "exploremanager.h"
#include "modrinthapi.h"

class ModrinthMod;
class ModrinthManager;
class ModrinthManagerModel : public QAbstractListModel
{
    Q_OBJECT
    friend class ModrinthManager;
public:
    enum Column { ModColumn, NameColumn, IDColumn, AuthorColumn, CategoryColumn, WebsiteColumn, UpdateDateColumn, CreateDateColumn, LoaderTypesColumn, DownloadCountColumn, SummaryColumn };
    ModrinthManagerModel(ModrinthManager *manager);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setItemHeight(int newItemHeight);

private:
    ModrinthManager *manager_;
    int itemHeight_ = 100;
};

class ModrinthManager : public ExploreManager
{
    Q_OBJECT
public:
    explicit ModrinthManager(QObject *parent = nullptr);

    void search(const QString name, const QList<GameVersion> &versions, ModLoaderType::Type type, const QList<QString> &categories, int sort);
    void searchMore();
    void refresh();

    ModrinthManagerModel *model() const override;
    const QList<ModrinthMod *> &mods() const;

private:
    ModrinthAPI api_;
    ModrinthManagerModel *model_;
    QList<ModrinthMod *> mods_;
    QString currentName_;
    int currentIndex_;
    QStringList currentCategoryIds_;
    QList<GameVersion> currentGameVersions_;
    ModLoaderType::Type currentType_;
    int currentSort_;
    bool hasMore_ = false;
    std::unique_ptr<Reply<QList<ModrinthModInfo>>> searchModsGetter_;

    void getModList() override;
};

#endif // MODRINTHMANAGER_H
