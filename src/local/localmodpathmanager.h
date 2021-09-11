#ifndef LOCALMODPATHMANAGER_H
#define LOCALMODPATHMANAGER_H

#include <QObject>

class LocalModPath;

class LocalModPathManager : public QObject
{
    Q_OBJECT
public:
    explicit LocalModPathManager(QObject *parent = nullptr);

    static LocalModPathManager *manager();

    static const QList<LocalModPath *> &pathList();
    static void setPathList(const QList<LocalModPath *> &newPathList);

    static void addPath(LocalModPath *path);
    static void removePath(LocalModPath *path);
    static void removePathAt(int i);
    static void insertPath(int i, LocalModPath *path);
    static void setPath(int i, LocalModPath *path);

    static void saveToConfig();

signals:
    void pathListUpdated();

private:
    void updateList();
    QList<LocalModPath*> pathList_;

};

#endif // LOCALMODPATHMANAGER_H
