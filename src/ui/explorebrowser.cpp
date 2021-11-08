#include "explorebrowser.h"

#include <QAction>
#include <QDesktopServices>
#include <QUrl>

ExploreBrowser::ExploreBrowser(QWidget *parent, const QIcon &icon, const QString &name, const QUrl &url) :
    Browser(parent),
    icon_(icon),
    name_(name),
    visitWebsiteAction_(new QAction(icon, tr("Visit %1").arg(name), this))
{
    connect(visitWebsiteAction_, &QAction::triggered, this, [=]{
        QDesktopServices::openUrl(url);
    });
}

QAction *ExploreBrowser::visitWebsiteAction() const
{
    return visitWebsiteAction_;
}

const QIcon &ExploreBrowser::icon() const
{
    return icon_;
}

const QString &ExploreBrowser::name() const
{
    return name_;
}
