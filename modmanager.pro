QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    modmanager/curseforgemod.cpp \
    modmanager/curseforgemodbrowser.cpp \
    modmanager/curseforgemodinfodialog.cpp \
    modmanager/curseforgemoditemwidget.cpp \
    modmanager/localmodbrowser.cpp \
    modmanager/localmodinfo.cpp \
    modmanager/localmoditemwidget.cpp \
    modmanager/main.cpp \
    modmanager/moddirinfo.cpp \
    modmanager/modmanager.cpp

HEADERS += \
    modmanager/curseforgemod.h \
    modmanager/curseforgemodbrowser.h \
    modmanager/curseforgemodinfodialog.h \
    modmanager/curseforgemoditemwidget.h \
    modmanager/localmodbrowser.h \
    modmanager/localmodinfo.h \
    modmanager/localmoditemwidget.h \
    modmanager/moddirinfo.h \
    modmanager/modmanager.h \
    util/qjsonutil.hpp

FORMS += \
    modmanager/curseforgemodbrowser.ui \
    modmanager/curseforgemodinfodialog.ui \
    modmanager/curseforgemoditemwidget.ui \
    modmanager/localmoditemwidget.ui \
    modmanager/modbrowserwidget.ui \
    modmanager/modmanager.ui

TRANSLATIONS += \
    languages/zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations
CONFIG += console

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#quazip: http://quazip.sourceforge.net/
include(3rdparty/quazip-0.7.3/quazip.pri)

#murmurhash: https://github.com/aappleby/smhasher
include(3rdparty/MurmurHash/murmurhash.pri)
