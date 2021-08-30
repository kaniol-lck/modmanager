#include "config.h"

#include <QTextCodec>

Config::Config() : QSettings("modmanager.ini", QSettings::IniFormat)
{}
