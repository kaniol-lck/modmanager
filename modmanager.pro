QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/src

SOURCES += \
    src/curseforge/curseforgeapi.cpp \
    src/curseforge/curseforgefileinfo.cpp \
    src/curseforge/curseforgemod.cpp \
    src/curseforge/curseforgemodinfo.cpp \
    src/local/localmod.cpp \
    src/modloadertype.cpp \
    src/modrinth/modrinthapi.cpp \
    src/modrinth/modrinthmod.cpp \
    src/modrinth/modrinthmodinfo.cpp \
    src/ui/curseforgefileitemwidget.cpp \
    src/ui/curseforgemodbrowser.cpp \
    src/ui/curseforgemodinfodialog.cpp \
    src/ui/curseforgemoditemwidget.cpp \
    src/gameversion.cpp \
    src/ui/localmodbrowser.cpp \
    src/ui/localmodbrowsersettingsdialog.cpp \
    src/local/localmodinfo.cpp \
    src/ui/localmodinfodialog.cpp \
    src/ui/localmoditemwidget.cpp \
    src/main.cpp \
    src/moddirinfo.cpp \
    src/ui/localmodupdatedialog.cpp \
    src/ui/modmanager.cpp \
    src/ui/modrinthmodbrowser.cpp \
    src/ui/modrinthmoditemwidget.cpp \
    src/util/downloader.cpp \
    src/util/downloaderthread.cpp \
    src/util/funcutil.cpp \
    src/util/datetimesortitem.cpp

HEADERS += \
    src/curseforge/curseforgeapi.h \
    src/curseforge/curseforgefileinfo.h \
    src/curseforge/curseforgemod.h \
    src/curseforge/curseforgemodinfo.h \
    src/local/localmod.h \
    src/modloadertype.h \
    src/modrinth/modrinthapi.h \
    src/modrinth/modrinthmod.h \
    src/modrinth/modrinthmodinfo.h \
    src/ui/curseforgefileitemwidget.h \
    src/ui/curseforgemodbrowser.h \
    src/ui/curseforgemodinfodialog.h \
    src/ui/curseforgemoditemwidget.h \
    src/gameversion.h \
    src/ui/localmodbrowser.h \
    src/ui/localmodbrowsersettingsdialog.h \
    src/local/localmodinfo.h \
    src/ui/localmodinfodialog.h \
    src/ui/localmoditemwidget.h \
    src/moddirinfo.h \
    src/ui/localmodupdatedialog.h \
    src/ui/modmanager.h \
    src/ui/modrinthmodbrowser.h \
    src/ui/modrinthmoditemwidget.h \
    src/util/downloader.h \
    src/util/downloaderthread.h \
    src/util/funcutil.h \
    src/util/tutil.hpp \
    src/util/datetimesortitem.h

FORMS += \
    src/ui/curseforgefileitemwidget.ui \
    src/ui/curseforgemodbrowser.ui \
    src/ui/curseforgemodinfodialog.ui \
    src/ui/curseforgemoditemwidget.ui \
    src/ui/localmodbrowser.ui \
    src/ui/localmodbrowsersettingsdialog.ui \
    src/ui/localmodinfodialog.ui \
    src/ui/localmoditemwidget.ui \
    src/ui/localmodupdatedialog.ui \
    src/ui/modmanager.ui \
    src/ui/modrinthmodbrowser.ui \
    src/ui/modrinthmoditemwidget.ui

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
