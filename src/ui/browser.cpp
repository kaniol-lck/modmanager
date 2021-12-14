#include "browser.h"

Browser::Browser(QWidget *parent) : QMainWindow(parent)
{}

void Browser::load()
{}

Browser::~Browser()
{}

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

