#include "config.h"

#include <QTextCodec>
#include <QDebug>

Config::Config() : QSettings("modmanager.ini", QSettings::IniFormat)
{
    setIniCodec(QTextCodec::codecForName("UTF-8"));
}
