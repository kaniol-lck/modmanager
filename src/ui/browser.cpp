#include "browser.h"

Browser::Browser(QWidget *parent) : QWidget(parent)
{}

Browser::~Browser()
{}

QWidget *Browser::infoWidget() const
{
    return nullptr;
}
