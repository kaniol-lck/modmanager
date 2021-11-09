QT       += core gui network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17
DEFINES += _LIBCPP_DISABLE_AVAILABILITY

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/src

SOURCES += \
    src/bmclapi.cpp \
    src/curseforge/curseforgeapi.cpp \
    src/curseforge/curseforgefileinfo.cpp \
    src/curseforge/curseforgemod.cpp \
    src/curseforge/curseforgemodinfo.cpp \
    src/download/abstractdownloader.cpp \
    src/download/downloadfileinfo.cpp \
    src/download/downloadmanager.cpp \
    src/download/qaria2.cpp \
    src/download/qaria2downloader.cpp \
    src/local/commonmodinfo.cpp \
    src/local/fabricmodinfo.cpp \
    src/local/forgemodinfo.cpp \
    src/local/idmapper.cpp \
    src/local/knownfile.cpp \
    src/local/localmod.cpp \
    src/local/localmodfile.cpp \
    src/local/localmodpath.cpp \
    src/local/localmodpathinfo.cpp \
    src/local/localmodpathmanager.cpp \
    src/modloadertype.cpp \
    src/modrinth/modrinthapi.cpp \
    src/modrinth/modrinthfileinfo.cpp \
    src/modrinth/modrinthmod.cpp \
    src/modrinth/modrinthmodinfo.cpp \
    src/optifine/optifineapi.cpp \
    src/optifine/optifinemod.cpp \
    src/optifine/optifinemodinfo.cpp \
    src/ui/browser.cpp \
    src/ui/curseforge/curseforgeimagepopup.cpp \
    src/ui/curseforge/curseforgemodinfowidget.cpp \
    src/ui/local/localmodinfowidget.cpp \
    src/ui/modrinth/modrinthmodinfowidget.cpp \
    src/ui/pageswitcher.cpp \
    src/replay/replayapi.cpp \
    src/replay/replaymod.cpp \
    src/replay/replaymodinfo.cpp \
    src/tag/localmodtags.cpp \
    src/tag/tag.cpp \
    src/tag/tagcategory.cpp \
    src/ui/aboutdialog.cpp \
    src/ui/browsermanagerdialog.cpp \
    src/ui/browserselectorwidget.cpp \
    src/ui/curseforge/curseforgefileitemwidget.cpp \
    src/ui/curseforge/curseforgemodbrowser.cpp \
    src/ui/curseforge/curseforgemoddialog.cpp \
    src/ui/curseforge/curseforgemoditemwidget.cpp \
    src/gameversion.cpp \
    src/ui/download/qaria2downloaderitemwidget.cpp \
    src/ui/download/downloadbrowser.cpp \
    src/ui/explorebrowser.cpp \
    src/ui/local/batchrenamedialog.cpp \
    src/ui/local/localmodbrowser.cpp \
    src/ui/local/localmodcheckdialog.cpp \
    src/ui/local/localmoddialog.cpp \
    src/ui/local/localmodfilter.cpp \
    src/ui/local/localmoditemwidget.cpp \
    src/main.cpp \
    src/ui/local/localmodmenu.cpp \
    src/ui/local/localmodpathsettingsdialog.cpp \
    src/ui/local/localmodupdatedialog.cpp \
    src/ui/modmanager.cpp \
    src/ui/modrinth/modrinthfileitemwidget.cpp \
    src/ui/modrinth/modrinthmodbrowser.cpp \
    src/ui/modrinth/modrinthmoddialog.cpp \
    src/ui/modrinth/modrinthmoditemwidget.cpp \
    src/ui/optifine/optifinemodbrowser.cpp \
    src/ui/optifine/optifinemoditemwidget.cpp \
    src/ui/preferences.cpp \
    src/ui/renamehighlighter.cpp \
    src/ui/replay/replaymodbrowser.cpp \
    src/ui/replay/replaymoditemwidget.cpp \
    src/ui/windowstitlebar.cpp \
    src/util/flowlayout.cpp \
    src/util/funcutil.cpp \
    src/util/datetimesortitem.cpp \
    src/util/localmodsortitem.cpp \
    src/util/mmlogger.cpp \
    src/util/smoothscrollbar.cpp \
    src/util/unclosedmenu.cpp \
    src/util/updatesourcedelegate.cpp \
    src/util/websiteicon.cpp \
    src/util/youdaotranslator.cpp

HEADERS += \
    src/bmclapi.h \
    src/config.hpp \
    src/curseforge/curseforgeapi.h \
    src/curseforge/curseforgefileinfo.h \
    src/curseforge/curseforgemod.h \
    src/curseforge/curseforgemodinfo.h \
    src/download/abstractdownloader.h \
    src/download/downloadfileinfo.h \
    src/download/downloadmanager.h \
    src/download/qaria2.h \
    src/download/qaria2downloader.h \
    src/local/commonmodinfo.h \
    src/local/fabricmodinfo.h \
    src/local/forgemodinfo.h \
    src/local/idmapper.h \
    src/local/knownfile.h \
    src/local/localmod.h \
    src/local/localmodfile.h \
    src/local/localmodpath.h \
    src/local/localmodpathinfo.h \
    src/local/localmodpathmanager.h \
    src/local/updatable.hpp \
    src/modloadertype.h \
    src/modrinth/modrinthapi.h \
    src/modrinth/modrinthfileinfo.h \
    src/modrinth/modrinthmod.h \
    src/modrinth/modrinthmodinfo.h \
    src/optifine/optifineapi.h \
    src/optifine/optifinemod.h \
    src/optifine/optifinemodinfo.h \
    src/ui/browser.h \
    src/ui/curseforge/curseforgeimagepopup.h \
    src/ui/curseforge/curseforgemodinfowidget.h \
    src/ui/local/localmodinfowidget.h \
    src/ui/modrinth/modrinthmodinfowidget.h \
    src/ui/pageswitcher.h \
    src/replay/replayapi.h \
    src/replay/replaymod.h \
    src/replay/replaymodinfo.h \
    src/tag/localmodtags.h \
    src/tag/tag.h \
    src/tag/tagcategory.h \
    src/ui/aboutdialog.h \
    src/ui/browsermanagerdialog.h \
    src/ui/browserselectorwidget.h \
    src/ui/curseforge/curseforgefileitemwidget.h \
    src/ui/curseforge/curseforgemodbrowser.h \
    src/ui/curseforge/curseforgemoddialog.h \
    src/ui/curseforge/curseforgemoditemwidget.h \
    src/gameversion.h \
    src/ui/download/qaria2downloaderitemwidget.h \
    src/ui/download/downloadbrowser.h \
    src/ui/explorebrowser.h \
    src/ui/local/batchrenamedialog.h \
    src/ui/local/localmodbrowser.h \
    src/ui/local/localmodcheckdialog.h \
    src/ui/local/localmoddialog.h \
    src/ui/local/localmodfilter.h \
    src/ui/local/localmoditemwidget.h \
    src/ui/local/localmodmenu.h \
    src/ui/local/localmodpathsettingsdialog.h \
    src/ui/local/localmodupdatedialog.h \
    src/ui/modmanager.h \
    src/ui/modrinth/modrinthfileitemwidget.h \
    src/ui/modrinth/modrinthmodbrowser.h \
    src/ui/modrinth/modrinthmoddialog.h \
    src/ui/modrinth/modrinthmoditemwidget.h \
    src/ui/optifine/optifinemodbrowser.h \
    src/ui/optifine/optifinemoditemwidget.h \
    src/ui/preferences.h \
    src/ui/renamehighlighter.h \
    src/ui/replay/replaymodbrowser.h \
    src/ui/replay/replaymoditemwidget.h \
    src/ui/windowstitlebar.h \
    src/util/WindowCompositionAttribute.h \
    src/util/flowlayout.h \
    src/util/funcutil.h \
    src/util/localmodsortitem.h \
    src/util/mmlogger.h \
    src/util/smoothscrollbar.h \
    src/util/tutil.hpp \
    src/util/datetimesortitem.h \
    src/util/unclosedmenu.h \
    src/util/updatesourcedelegate.h \
    src/util/websiteicon.h \
    src/util/youdaotranslator.h \
    src/version.h

FORMS += \
    src/ui/aboutdialog.ui \
    src/ui/browsermanagerdialog.ui \
    src/ui/browserselectorwidget.ui \
    src/ui/curseforge/curseforgefileitemwidget.ui \
    src/ui/curseforge/curseforgeimagepopup.ui \
    src/ui/curseforge/curseforgemodbrowser.ui \
    src/ui/curseforge/curseforgemoddialog.ui \
    src/ui/curseforge/curseforgemodinfowidget.ui \
    src/ui/curseforge/curseforgemoditemwidget.ui \
    src/ui/download/qaria2downloaderitemwidget.ui \
    src/ui/download/downloadbrowser.ui \
    src/ui/local/batchrenamedialog.ui \
    src/ui/local/localmodbrowser.ui \
    src/ui/local/localmodcheckdialog.ui \
    src/ui/local/localmoddialog.ui \
    src/ui/local/localmoditemwidget.ui \
    src/ui/local/localmodpathsettingsdialog.ui \
    src/ui/local/localmodupdatedialog.ui \
    src/ui/local/localmodinfowidget.ui \
    src/ui/modmanager.ui \
    src/ui/modrinth/modrinthfileitemwidget.ui \
    src/ui/modrinth/modrinthmodbrowser.ui \
    src/ui/modrinth/modrinthmoddialog.ui \
    src/ui/modrinth/modrinthmodinfowidget.ui \
    src/ui/modrinth/modrinthmoditemwidget.ui \
    src/ui/optifine/optifinemodbrowser.ui \
    src/ui/optifine/optifinemoditemwidget.ui \
    src/ui/preferences.ui \
    src/ui/replay/replaymodbrowser.ui \
    src/ui/replay/replaymoditemwidget.ui \
    src/ui/windowstitlebar.ui

TRANSLATIONS += \
    languages/zh_CN.ts \
    languages/zh_TW.ts
CONFIG += lrelease
CONFIG += embed_translations

#quazip: http://quazip.sourceforge.net/
#include(3rdparty/quazip-0.7.3/quazip.pri)

#murmurhash: https://github.com/aappleby/smhasher
include(3rdparty/MurmurHash/murmurhash.pri)

#cpp-semver: https://github.com/easz/cpp-semver
include(3rdparty/cpp-semver/cpp-semver.pri)

#tomlplusplus: https://github.com/marzer/tomlplusplus
include(3rdparty/tomlplusplus/tomlplusplus.pri)

RESOURCES += \
    src/icons/breeze/breeze-modmanager.qrc \
    src/images/image.qrc

TARGET = ModManager
TEMPLATE = app

unix {
  isEmpty(PREFIX) {
    PREFIX = /usr
  }

  isEmpty(BINDIR) {
    BINDIR = $$PREFIX/bin
  }

  isEmpty(DATADIR) {
    DATADIR = $$PREFIX/share
  }

  INSTALLS += desktop icon

  target.path = $$INSTROOT$$BINDIR

  desktop.path = $$DATADIR/applications
  desktop.files += src/ModManager.desktop

  icon.path = $$DATADIR/icons/hicolor/256x256/apps
  icon.files += src/modmanager.png
}

INSTALLS += target

CONFIG(debug, debug|release) {
#    QMAKE_CXXFLAGS_DEBUG += -g3 -O0
    message("Currently in DEBUG mode.")
} else {
    DEFINES += QT_NO_DEBUG QT_NO_DEBUG_OUTPUT
    message("Currently in RELEASE mode.")
}

DISTFILES += \
    src/ModManager.desktop \
    src/modmanager.png

win32: RC_ICONS = src/modmanager.ico

#dependencies
unix {
  message("unix-like build")
  exists(/usr/lib/x86_64-linux-gnu/libquazip5.a) LIBS += -L$$quote(/usr/lib/x86_64-linux-gnu) -lquazip5
  LIBS += -L$$quote(/usr/local/lib) -laria2 -lquazip1-qt5
  INCLUDEPATH += /usr/include/QuaZip-Qt5-1.1/quazip

  message(HEADERS)
  exists(/usr/include/KF5/KWindowSystem){
    message("under KDE")
    DEFINES += DE_KDE
    LIBS += -lKF5WindowSystem
    INCLUDEPATH += /usr/include/KF5/KWindowSystem
  }
}

macx {
  message("macos build")
  INCLUDEPATH += /usr/local/include
  INCLUDEPATH += /usr/local/include/QuaZip-Qt5-1.1/quazip
}

win32 {
  LIBS += -ldwmapi
  contains(QMAKE_HOST.arch, x86_64) {
    message("win32 x86_64 build")
    INCLUDEPATH += C:/msys64/mingw64/include
    INCLUDEPATH += C:/msys64/mingw64/include/quazip5
    LIBS += -L$$quote(C:/msys64/mingw64/lib) -laria2 -lquazip5
  } else {
    message("win32 x86 build")
    INCLUDEPATH += C:/msys64/mingw32/include
    INCLUDEPATH += C:/msys64/mingw32/include/quazip5
    LIBS += -L$$quote(C:/msys64/mingw32/lib) -laria2 -lquazip5
  }
}
