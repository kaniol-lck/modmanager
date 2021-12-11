#ifndef EXPLOREBROWSER_H
#define EXPLOREBROWSER_H

#include "ui/browser.h"
#include "local/localmodpathinfo.h"

class QMenu;
class ExploreBrowser : public Browser
{
    Q_OBJECT
public:
    explicit ExploreBrowser(QWidget *parent, const QIcon &icon, const QString &name, const QUrl &url);
    virtual ~ExploreBrowser() = default;

    virtual ExploreBrowser *another() = 0;

    QList<QAction *> modActions() const override;
    QList<QAction *> pathActions() const override;

    QAction *visitWebsiteAction() const;
    QIcon icon() const override;
    QString name() const override;
public slots:
    virtual void refresh() = 0;
    virtual void searchModByPathInfo(const LocalModPathInfo &info) = 0;
    virtual void updateUi() = 0;

    void openDialog();
signals:
protected:
    QMenu *modMenu_;
    QMenu *pathMenu_;

private:
    QIcon icon_;
    QString name_;
    QAction *visitWebsiteAction_;
};

#endif // EXPLOREBROWSER_H
