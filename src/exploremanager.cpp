#include "exploremanager.h"

ExploreManager::ExploreManager(QObject *parent)
    : QObject{parent}
{}

void ExploreManager::refresh()
{
    getModList();
}
