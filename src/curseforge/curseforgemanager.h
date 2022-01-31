#ifndef CURSEFORGEMANAGER_H
#define CURSEFORGEMANAGER_H

#include <QAbstractListModel>
#include <QObject>
#include <QSortFilterProxyModel>

#include "curseforgeapi.h"

class CurseforgeMod;
class CurseforgeManagerModel;

class CurseforgeManager : public QObject
{
    Q_OBJECT
public:
    enum Column { ModColumn };

    CurseforgeManager(QObject *parent = nullptr, CurseforgeAPI::Section sectionId = CurseforgeAPI::Mod);

    void search(const QString &name, int categoryId, GameVersion version, int sort);
    void searchMore();
    void refresh();

    CurseforgeManagerModel *model() const;
    const QList<CurseforgeMod *> &mods() const;

    void setSectionId(CurseforgeAPI::Section newSectionId);

signals:
    void searchStarted();
    void searchFinished(bool success = true);
    void scrollToTop();

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

    void getModList();
};

class CurseforgeManagerModel : public QAbstractListModel
{
    Q_OBJECT
    friend class CurseforgeManager;
public:
    CurseforgeManagerModel(CurseforgeManager *manager);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setItemHeight(int newItemHeight);

private:
    CurseforgeManager *manager_;
    int itemHeight_ = 100;
};

class CurseforgeManagerProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    CurseforgeManagerProxyModel(QObject *parent = nullptr);

    void setLoaderType(ModLoaderType::Type newLoaderType);

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &) const override;

private:
    ModLoaderType::Type loaderType_ = ModLoaderType::Any;
};

#endif // CURSEFORGEMANAGER_H
