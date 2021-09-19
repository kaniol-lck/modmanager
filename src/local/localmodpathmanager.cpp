#include "localmodpathmanager.h"

#include "localmodpath.h"
#include "config.h"

LocalModPathManager::LocalModPathManager(QObject *parent) : QObject(parent)
{
    Config config;
    for(const auto &pathInfo : config.getLocalPathList()){
        auto path = new LocalModPath(this, LocalModPathInfo::fromVariant(pathInfo), true);
        pathList_ << path;
        connect(path, &LocalModPath::infoUpdated, this, &LocalModPathManager::updateList);
    }
}

LocalModPathManager *LocalModPathManager::manager()
{
    static LocalModPathManager manager;
    return &manager;
}

const QList<LocalModPath *> &LocalModPathManager::pathList()
{
    return manager()->pathList_;
}

void LocalModPathManager::setPathList(const QList<LocalModPath *> &newPathList)
{
    for(const auto &path : newPathList)
        path->setParent(manager());
    manager()->pathList_ = newPathList;
    manager()->updateList();
}

void LocalModPathManager::addPath(LocalModPath *path)
{
    path->setParent(manager());
    connect(path, &LocalModPath::infoUpdated, manager(), &LocalModPathManager::updateList);
    manager()->pathList_ << path;
    manager()->updateList();
}

void LocalModPathManager::removePath(LocalModPath *path)
{
    manager()->pathList_.removeAll(path);
    manager()->updateList();
}

void LocalModPathManager::removePathAt(int i)
{
    manager()->pathList_.removeAt(i);
    manager()->updateList();
}

void LocalModPathManager::insertPath(int i, LocalModPath *path)
{
    path->setParent(manager());
    manager()->pathList_.insert(i, path);
    manager()->updateList();
}

void LocalModPathManager::setPath(int i, LocalModPath *path)
{
    path->setParent(manager());
    manager()->pathList_[i] = path;
}

void LocalModPathManager::saveToConfig()
{
    Config config;
    QList<QVariant> list;
    for(const auto &path : qAsConst(manager()->pathList_))
        list << path->info().toVariant();
    config.setLocalPathList(list);
}

void LocalModPathManager::updateList()
{
    emit pathListUpdated();
    saveToConfig();
}
