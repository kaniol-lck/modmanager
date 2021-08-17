QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/curseforgemod.cpp \
    src/curseforgemodbrowser.cpp \
    src/curseforgemodinfodialog.cpp \
    src/curseforgemoditemwidget.cpp \
    src/gameversion.cpp \
    src/localmodbrowser.cpp \
    src/localmodbrowsersettingsdialog.cpp \
    src/localmodinfo.cpp \
    src/localmoditemwidget.cpp \
    src/main.cpp \
    src/moddirinfo.cpp \
    src/modmanager.cpp \
    util/downloader.cpp \
    util/downloaderthread.cpp \
    util/funcutil.cpp

HEADERS += \
    src/curseforgemod.h \
    src/curseforgemodbrowser.h \
    src/curseforgemodinfodialog.h \
    src/curseforgemoditemwidget.h \
    src/gameversion.h \
    src/localmodbrowser.h \
    src/localmodbrowsersettingsdialog.h \
    src/localmodinfo.h \
    src/localmoditemwidget.h \
    src/moddirinfo.h \
    src/modmanager.h \
    util/downloader.h \
    util/downloaderthread.h \
    util/funcutil.h \
    util/tutil.hpp

FORMS += \
    src/curseforgemodbrowser.ui \
    src/curseforgemodinfodialog.ui \
    src/curseforgemoditemwidget.ui \
    src/localmodbrowser.ui \
    src/localmodbrowsersettingsdialog.ui \
    src/localmoditemwidget.ui \
    src/modmanager.ui

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
