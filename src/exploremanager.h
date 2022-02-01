#ifndef EXPLOREMANAGER_H
#define EXPLOREMANAGER_H

#include <QAbstractItemModel>
#include <QObject>

class ExploreManager : public QObject
{
    Q_OBJECT
public:
    explicit ExploreManager(QObject *parent = nullptr);

    void refresh();

    virtual QAbstractItemModel *model() const = 0;

signals:
    void searchStarted();
    void searchFinished(bool success = true);
    void scrollToTop();

private:
    virtual void getModList() = 0;
};

#endif // EXPLOREMANAGER_H
