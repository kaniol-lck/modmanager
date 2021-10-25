#ifndef EXPLOREBROWSER_H
#define EXPLOREBROWSER_H

#include <QWidget>

#include "local/localmodpathinfo.h"

class ExploreBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit ExploreBrowser(QWidget *parent, const QIcon &icon, const QString &name, const QUrl &url);
    virtual ~ExploreBrowser() = default;

    QAction *visitWebsiteAction() const;
public slots:
    virtual void refresh() = 0;
    virtual void searchModByPathInfo(const LocalModPathInfo &info) = 0;
    virtual void updateUi() = 0;
signals:
private:
    QAction *visitWebsiteAction_;
};

#endif // EXPLOREBROWSER_H
