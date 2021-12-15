#include "browserdialog.h"
#include "explorebrowser.h"

#include <QAction>
#include <QDesktopServices>
#include <QDockWidget>
#include <QMenu>
#include <QUrl>

ExploreBrowser::ExploreBrowser(QWidget *parent, const QIcon &icon, const QString &name, const QUrl &url) :
    Browser(parent),
    modMenu_(new QMenu(this)),
    pathMenu_(new QMenu(this)),
    icon_(icon),
    name_(name),
    visitWebsiteAction_(new QAction(icon, tr("Visit %1").arg(name), this))
{
    connect(visitWebsiteAction_, &QAction::triggered, this, [=]{
        QDesktopServices::openUrl(url);
    });
    pathMenu_->addAction(QIcon::fromTheme("view-refresh"), tr("Refresh Mods"), this, &ExploreBrowser::refresh)
            ->setShortcut(QKeySequence(Qt::Key_F5));
    pathMenu_->addAction(visitWebsiteAction_);
    pathMenu_->addAction(QIcon::fromTheme("window-new"), tr("Open in New Dialog"), this, &ExploreBrowser::openDialog);
}

QList<QAction *> ExploreBrowser::modActions() const
{
    return modMenu_->actions();
}

QList<QAction *> ExploreBrowser::pathActions() const
{
    return pathMenu_->actions();
}

QAction *ExploreBrowser::visitWebsiteAction() const
{
    return visitWebsiteAction_;
}

QIcon ExploreBrowser::icon() const
{
    return icon_;
}

QString ExploreBrowser::name() const
{
    return name_;
}

void ExploreBrowser::openDialog()
{
//    auto dialog = new BrowserDialog(nullptr, another());
//    dialog->setWindowTitle(name_);
//    dialog->setWindowIcon(icon_);
//    dialog->show();
    another()->show();
}
