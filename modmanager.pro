QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/src

SOURCES += \
    src/config.cpp \
    src/curseforge/curseforgeapi.cpp \
    src/curseforge/curseforgefileinfo.cpp \
    src/curseforge/curseforgemod.cpp \
    src/curseforge/curseforgemodinfo.cpp \
    src/download/downloader.cpp \
    src/download/downloaderthread.cpp \
    src/download/downloadfileinfo.cpp \
    src/download/downloadmanager.cpp \
    src/download/moddownloader.cpp \
    src/local/localmod.cpp \
    src/local/localmodpath.cpp \
    src/local/localmodpathinfo.cpp \
    src/local/localmodpathmanager.cpp \
    src/modloadertype.cpp \
    src/modrinth/modrinthapi.cpp \
    src/modrinth/modrinthfileinfo.cpp \
    src/modrinth/modrinthmod.cpp \
    src/modrinth/modrinthmodinfo.cpp \
    src/ui/browsermanagerdialog.cpp \
    src/ui/curseforgefileitemwidget.cpp \
    src/ui/curseforgemodbrowser.cpp \
    src/ui/curseforgemodinfodialog.cpp \
    src/ui/curseforgemoditemwidget.cpp \
    src/gameversion.cpp \
    src/ui/downloadbrowser.cpp \
    src/ui/downloaderitemwidget.cpp \
    src/ui/localmodbrowser.cpp \
    src/ui/localmodbrowsersettingsdialog.cpp \
    src/local/localmodinfo.cpp \
    src/ui/localmodinfodialog.cpp \
    src/ui/localmoditemwidget.cpp \
    src/main.cpp \
    src/ui/localmodupdatedialog.cpp \
    src/ui/modmanager.cpp \
    src/ui/modrinthfileitemwidget.cpp \
    src/ui/modrinthmodbrowser.cpp \
    src/ui/modrinthmodinfodialog.cpp \
    src/ui/modrinthmoditemwidget.cpp \
    src/ui/preferences.cpp \
    src/util/funcutil.cpp \
    src/util/datetimesortitem.cpp \
    src/util/localmodsortitem.cpp \
    src/util/updatesourcedelegate.cpp

HEADERS += \
    src/config.h \
    src/curseforge/curseforgeapi.h \
    src/curseforge/curseforgefileinfo.h \
    src/curseforge/curseforgemod.h \
    src/curseforge/curseforgemodinfo.h \
    src/download/downloader.h \
    src/download/downloaderthread.h \
    src/download/downloadfileinfo.h \
    src/download/downloadmanager.h \
    src/download/moddownloader.h \
    src/local/localmod.h \
    src/local/localmodpath.h \
    src/local/localmodpathinfo.h \
    src/local/localmodpathmanager.h \
    src/modloadertype.h \
    src/modrinth/modrinthapi.h \
    src/modrinth/modrinthfileinfo.h \
    src/modrinth/modrinthmod.h \
    src/modrinth/modrinthmodinfo.h \
    src/ui/browsermanagerdialog.h \
    src/ui/curseforgefileitemwidget.h \
    src/ui/curseforgemodbrowser.h \
    src/ui/curseforgemodinfodialog.h \
    src/ui/curseforgemoditemwidget.h \
    src/gameversion.h \
    src/ui/downloadbrowser.h \
    src/ui/downloaderitemwidget.h \
    src/ui/localmodbrowser.h \
    src/ui/localmodbrowsersettingsdialog.h \
    src/local/localmodinfo.h \
    src/ui/localmodinfodialog.h \
    src/ui/localmoditemwidget.h \
    src/ui/localmodupdatedialog.h \
    src/ui/modmanager.h \
    src/ui/modrinthfileitemwidget.h \
    src/ui/modrinthmodbrowser.h \
    src/ui/modrinthmodinfodialog.h \
    src/ui/modrinthmoditemwidget.h \
    src/ui/preferences.h \
    src/util/funcutil.h \
    src/util/localmodsortitem.h \
    src/util/tutil.hpp \
    src/util/datetimesortitem.h \
    src/util/updatesourcedelegate.h

FORMS += \
    src/ui/browsermanagerdialog.ui \
    src/ui/curseforgefileitemwidget.ui \
    src/ui/curseforgemodbrowser.ui \
    src/ui/curseforgemodinfodialog.ui \
    src/ui/curseforgemoditemwidget.ui \
    src/ui/downloadbrowser.ui \
    src/ui/downloaderitemwidget.ui \
    src/ui/localmodbrowser.ui \
    src/ui/localmodbrowsersettingsdialog.ui \
    src/ui/localmodinfodialog.ui \
    src/ui/localmoditemwidget.ui \
    src/ui/localmodupdatedialog.ui \
    src/ui/modmanager.ui \
    src/ui/modrinthfileitemwidget.ui \
    src/ui/modrinthmodbrowser.ui \
    src/ui/modrinthmodinfodialog.ui \
    src/ui/modrinthmoditemwidget.ui \
    src/ui/preferences.ui

TRANSLATIONS += \
    languages/zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#quazip: http://quazip.sourceforge.net/
include(3rdparty/quazip-0.7.3/quazip.pri)

#murmurhash: https://github.com/aappleby/smhasher
include(3rdparty/MurmurHash/murmurhash.pri)

RESOURCES += \
    src/images/image.qrc
