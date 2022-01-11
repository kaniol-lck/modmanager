#include "downloadpathselectmenu.h"

#include <QFileDialog>
#include <QMenu>

#include "local/localmodpath.h"
#include "local/localmodpathmanager.h"
#include "util/funcutil.h"

DownloadPathSelectMenu::DownloadPathSelectMenu(QWidget *parent) :
    QMenu(parent)
{
    connect(this, &QMenu::aboutToShow, this, &DownloadPathSelectMenu::onAboutToShow);
    menuAction()->setText(tr("Save To: %1").arg(tr("Default Download Path")));
    connect(menuAction(), &QAction::triggered, this, &QMenu::show);
    setIcon(QIcon::fromTheme("download"));
}

void DownloadPathSelectMenu::onAboutToShow()
{
    clear();
    addAction(QIcon::fromTheme("folder"), tr("Open Current Save Folder..."), this, [=]{
        if(downloadPath_)
            openFolder(downloadPath_->info().path());
        else
            openFolder(Config().getDownloadPath());
    });
    addSeparator();
    addAction(tr("Default Download Path"), this, [=]{
        setDownloadPath(nullptr);
    });
    addAction(tr("Change Default Download Path..."), this, [=]{
        auto dir = QFileDialog::getExistingDirectory(parentWidget(), tr("Change Default Download Path..."), Config().getDownloadPath());
        if(!dir.isEmpty()) Config().setDownloadPath(dir);
        if(!downloadPath_) emit DownloadPathChanged();
    });
    addSeparator();
    addSection(tr("Local Mod Paths"));
    for(auto &&path : LocalModPathManager::manager()->pathList())
        addAction(path->icon(), path->displayName(), this, [=]{
            setDownloadPath(path);
        });
}

void DownloadPathSelectMenu::setDownloadPath(LocalModPath *newDownloadPath)
{
    downloadPath_ = newDownloadPath;
    if(downloadPath_)
        menuAction()->setText(tr("Save To: %1").arg(downloadPath_->displayName()));
    else
        menuAction()->setText(tr("Save To: %1").arg(tr("Default Download Path")));
    emit DownloadPathChanged();
}

LocalModPath *DownloadPathSelectMenu::downloadPath() const
{
    return downloadPath_;
}
