#include "browser.h"

Browser::Browser(QWidget *parent) : QWidget(parent)
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
