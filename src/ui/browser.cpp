#include "browser.h"

#include <QDockWidget>

#include "config.hpp"

Browser::Browser(QWidget *parent) : QMainWindow(parent)
{}

void Browser::show()
{
    setWindowTitle(name());
    setWindowIcon(icon());
    //TODO
    if(infoWidget()){
        auto infoDock = new QDockWidget(tr("Infomation"), this);
        infoDock->setWidget(infoWidget());
        infoDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
        addDockWidget(Qt::RightDockWidgetArea, infoDock);
    }
    if(fileListWidget()){
        auto fileListDock = new QDockWidget(tr("File List"), this);
        fileListDock->setWidget(fileListWidget());
        fileListDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
        addDockWidget(Qt::RightDockWidgetArea, fileListDock);
    }
    load();
    QMainWindow::show();
}

void Browser::load()
{}

void Browser::updateUi()
{}

Browser::~Browser()
{
    Config config;
    config.setBrowserWindowState(this, saveGeometry());
    config.setBrowserWindowState(this, saveState());
}

QWidget *Browser::infoWidget() const
{
    return nullptr;
}

QWidget *Browser::fileListWidget() const
{
    return nullptr;
}

QList<QAction *> Browser::modActions() const
{
    return {};
}

QList<QAction *> Browser::pathActions() const
{
    return {};
}

void Browser::onModMenuAboutToShow()
{}

void Browser::onModMenuAboutToHide()
{}
