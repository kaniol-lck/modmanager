QT       += core gui network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17
DEFINES += QT_NO_VERSION_TAGGING
DEFINES += _LIBCPP_DISABLE_AVAILABILITY

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/src

SOURCES += \
    src/bmclapi.cpp \
    src/curseforge/curseforgeapi.cpp \
    src/curseforge/curseforgecategoryinfo.cpp \
    src/curseforge/curseforgedependencyinfo.cpp \
    src/curseforge/curseforgefile.cpp \
    src/curseforge/curseforgefileinfo.cpp \
    src/curseforge/curseforgemanager.cpp \
    src/curseforge/curseforgemod.cpp \
    src/curseforge/curseforgemodcacheinfo.cpp \
    src/curseforge/curseforgemodinfo.cpp \
    src/curseforge/curseforgemodinfocaches.cpp \
    src/download/abstractdownloader.cpp \
    src/download/assetcache.cpp \
    src/download/downloadfileinfo.cpp \
    src/download/downloadmanager.cpp \
    src/download/qaria2.cpp \
    src/download/qaria2downloader.cpp \
    src/exploremanager.cpp \
    src/github/githubapi.cpp \
    src/github/githubfileinfo.cpp \
    src/github/githubmanager.cpp \
    src/github/githubrelease.cpp \
    src/github/githubreleaseinfo.cpp \
    src/github/githubrepoinfo.cpp \
    src/local/commonmodinfo.cpp \
    src/local/fabricmodinfo.cpp \
    src/local/forgemodinfo.cpp \
    src/local/idmapper.cpp \
    src/local/knownfile.cpp \
    src/local/localfilelinker.cpp \
    src/local/localmod.cpp \
    src/local/localmodfile.cpp \
    src/local/localmodpath.cpp \
    src/local/localmodpathinfo.cpp \
    src/local/localmodpathmanager.cpp \
    src/local/modfilerenamer.cpp \
    src/modloadertype.cpp \
    src/modrinth/modrinthapi.cpp \
    src/modrinth/modrinthfileinfo.cpp \
    src/modrinth/modrinthmanager.cpp \
    src/modrinth/modrinthmod.cpp \
    src/modrinth/modrinthmodinfo.cpp \
    src/modwebsitetype.cpp \
    src/optifine/optifineapi.cpp \
    src/optifine/optifinemanager.cpp \
    src/optifine/optifinemod.cpp \
    src/optifine/optifinemodinfo.cpp \
    src/qss/stylesheets.cpp \
    src/replay/replaymanager.cpp \
    src/tag/tagable.cpp \
    src/ui/browser.cpp \
    src/ui/browserdialog.cpp \
    src/ui/curseforge/curseforgefilelistwidget.cpp \
    src/ui/curseforge/curseforgeimagepopup.cpp \
    src/ui/curseforge/curseforgemodinfowidget.cpp \
    src/ui/datetimetext.cpp \
    src/ui/dockwidgetcontent.cpp \
    src/ui/download/adddownloaddialog.cpp \
    src/ui/download/downloaderinfowidget.cpp \
    src/ui/download/downloaderspeedwidget.cpp \
    src/ui/download/downloadstatusbarwidget.cpp \
    src/ui/downloadpathselectmenu.cpp \
    src/ui/explorestatusbarwidget.cpp \
    src/ui/framelesswrapper.cpp \
    src/ui/github/githubfileitemwidget.cpp \
    src/ui/github/githubfilelistwidget.cpp \
    src/ui/github/githubreleaseinfowidget.cpp \
    src/ui/github/githubreleaseitemwidget.cpp \
    src/ui/github/githubrepobrowser.cpp \
    src/ui/local/exportmanifestjsondialog.cpp \
    src/ui/local/importcurseforgemodpackdialog.cpp \
    src/ui/local/localfilelistwidget.cpp \
    src/ui/local/localmodfileitemwidget.cpp \
    src/ui/local/localmodinfowidget.cpp \
    src/ui/local/localmodpathmodel.cpp \
    src/ui/local/localstatusbarwidget.cpp \
    src/ui/menutoolbar.cpp \
    src/ui/modmanagerupdatedialog.cpp \
    src/ui/modrinth/modrinthfilelistwidget.cpp \
    src/ui/modrinth/modrinthmodinfowidget.cpp \
    src/ui/pageswitcher.cpp \
    src/replay/replayapi.cpp \
    src/replay/replaymod.cpp \
    src/replay/replaymodinfo.cpp \
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
    src/ui/renamepatternedit.cpp \
    src/ui/replay/replaymodbrowser.cpp \
    src/ui/replay/replaymoditemwidget.cpp \
    src/ui/tagsflowwidget.cpp \
    src/ui/tagswidget.cpp \
    src/ui/windowstitlebar.cpp \
    src/util/checksheet.cpp \
    src/util/flowlayout.cpp \
    src/util/funcutil.cpp \
    src/util/datetimesortitem.cpp \
    src/util/localmodsortitem.cpp \
    src/util/mmlogger.cpp \
    src/util/smoothscrollbar.cpp \
    src/util/unclosedmenu.cpp \
    src/util/websiteicon.cpp \
    src/util/youdaotranslator.cpp

HEADERS += \
    src/bmclapi.h \
    src/config.hpp \
    src/curseforge/curseforgeapi.h \
    src/curseforge/curseforgecategoryinfo.h \
    src/curseforge/curseforgedependencyinfo.h \
    src/curseforge/curseforgefile.h \
    src/curseforge/curseforgefileinfo.h \
    src/curseforge/curseforgemanager.h \
    src/curseforge/curseforgemod.h \
    src/curseforge/curseforgemodcacheinfo.h \
    src/curseforge/curseforgemodinfo.h \
    src/curseforge/curseforgemodinfocaches.h \
    src/download/abstractdownloader.h \
    src/download/assetcache.h \
    src/download/downloadfileinfo.h \
    src/download/downloadmanager.h \
    src/download/qaria2.h \
    src/download/qaria2downloader.h \
    src/exploremanager.h \
    src/github/githubapi.h \
    src/github/githubfileinfo.h \
    src/github/githubmanager.h \
    src/github/githubrelease.h \
    src/github/githubreleaseinfo.h \
    src/github/githubrepoinfo.h \
    src/local/commonmodinfo.h \
    src/local/fabricmodinfo.h \
    src/local/forgemodinfo.h \
    src/local/idmapper.h \
    src/local/knownfile.h \
    src/local/localfilelinker.h \
    src/local/localmod.h \
    src/local/localmodfile.h \
    src/local/localmodpath.h \
    src/local/localmodpathinfo.h \
    src/local/localmodpathmanager.h \
    src/local/modfilerenamer.h \
    src/local/updater.hpp \
    src/modloadertype.h \
    src/modrinth/modrinthapi.h \
    src/modrinth/modrinthfileinfo.h \
    src/modrinth/modrinthmanager.h \
    src/modrinth/modrinthmod.h \
    src/modrinth/modrinthmodinfo.h \
    src/modwebsitetype.h \
    src/network/reply.hpp \
    src/optifine/optifineapi.h \
    src/optifine/optifinemanager.h \
    src/optifine/optifinemod.h \
    src/optifine/optifinemodinfo.h \
    src/qss/stylesheets.h \
    src/replay/replaymanager.h \
    src/tag/tagable.h \
    src/ui/browser.h \
    src/ui/browserdialog.h \
    src/ui/curseforge/curseforgefilelistwidget.h \
    src/ui/curseforge/curseforgeimagepopup.h \
    src/ui/curseforge/curseforgemodinfowidget.h \
    src/ui/datetimetext.h \
    src/ui/dockwidgetcontent.h \
    src/ui/download/adddownloaddialog.h \
    src/ui/download/downloaderinfowidget.h \
    src/ui/download/downloaderspeedwidget.h \
    src/ui/download/downloadstatusbarwidget.h \
    src/ui/downloadpathselectmenu.h \
    src/ui/explorestatusbarwidget.h \
    src/ui/framelesswrapper.h \
    src/ui/github/githubfileitemwidget.h \
    src/ui/github/githubfilelistwidget.h \
    src/ui/github/githubreleaseinfowidget.h \
    src/ui/github/githubreleaseitemwidget.h \
    src/ui/github/githubrepobrowser.h \
    src/ui/local/exportmanifestjsondialog.h \
    src/ui/local/importcurseforgemodpackdialog.h \
    src/ui/local/localfilelistwidget.h \
    src/ui/local/localmodfileitemwidget.h \
    src/ui/local/localmodinfowidget.h \
    src/ui/local/localmodpathmodel.h \
    src/ui/local/localstatusbarwidget.h \
    src/ui/menutoolbar.h \
    src/ui/modmanagerupdatedialog.h \
    src/ui/modrinth/modrinthfilelistwidget.h \
    src/ui/modrinth/modrinthmodinfowidget.h \
    src/ui/pageswitcher.h \
    src/replay/replayapi.h \
    src/replay/replaymod.h \
    src/replay/replaymodinfo.h \
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
    src/ui/renamepatternedit.h \
    src/ui/replay/replaymodbrowser.h \
    src/ui/replay/replaymoditemwidget.h \
    src/ui/tagsflowwidget.h \
    src/ui/tagswidget.h \
    src/ui/windowstitlebar.h \
    src/util/WindowCompositionAttribute.h \
    src/util/checksheet.h \
    src/util/flowlayout.h \
    src/util/funcutil.h \
    src/util/localmodsortitem.h \
    src/util/mmlogger.h \
    src/util/smoothscrollbar.h \
    src/util/tutil.hpp \
    src/util/datetimesortitem.h \
    src/util/unclosedmenu.h \
    src/util/websiteicon.h \
    src/util/youdaotranslator.h \
    src/version.h

FORMS += \
    src/ui/curseforge/curseforgemodbrowser.ui \
    src/ui/datetimetext.ui \
    src/ui/download/adddownloaddialog.ui \
    src/ui/download/downloadbrowser.ui \
    src/ui/download/downloaderinfowidget.ui \
    src/ui/download/downloadstatusbarwidget.ui \
    src/ui/explorestatusbarwidget.ui \
    src/ui/github/githubfileitemwidget.ui \
    src/ui/github/githubfilelistwidget.ui \
    src/ui/github/githubreleaseinfowidget.ui \
    src/ui/github/githubreleaseitemwidget.ui \
    src/ui/github/githubrepobrowser.ui \
    src/ui/local/exportmanifestjsondialog.ui \
    src/ui/local/importcurseforgemodpackdialog.ui \
    src/ui/local/localmodbrowser.ui \
    src/ui/local/localstatusbarwidget.ui \
    src/ui/aboutdialog.ui \
    src/ui/browserdialog.ui \
    src/ui/browsermanagerdialog.ui \
    src/ui/browserselectorwidget.ui \
    src/ui/curseforge/curseforgefileitemwidget.ui \
    src/ui/curseforge/curseforgefilelistwidget.ui \
    src/ui/curseforge/curseforgeimagepopup.ui \
    src/ui/curseforge/curseforgemodbrowser.ui \
    src/ui/curseforge/curseforgemoddialog.ui \
    src/ui/curseforge/curseforgemodinfowidget.ui \
    src/ui/curseforge/curseforgemoditemwidget.ui \
    src/ui/download/qaria2downloaderitemwidget.ui \
    src/ui/download/downloadbrowser.ui \
    src/ui/local/batchrenamedialog.ui \
    src/ui/local/localfilelistwidget.ui \
    src/ui/local/localmodbrowser.ui \
    src/ui/local/localmodcheckdialog.ui \
    src/ui/local/localmoddialog.ui \
    src/ui/local/localmodfileitemwidget.ui \
    src/ui/local/localmoditemwidget.ui \
    src/ui/local/localmodpathsettingsdialog.ui \
    src/ui/local/localmodupdatedialog.ui \
    src/ui/local/localmodinfowidget.ui \
    src/ui/modmanager.ui \
    src/ui/modmanagerupdatedialog.ui \
    src/ui/modrinth/modrinthfileitemwidget.ui \
    src/ui/modrinth/modrinthfilelistwidget.ui \
    src/ui/modrinth/modrinthmodbrowser.ui \
    src/ui/modrinth/modrinthmodbrowser.ui \
    src/ui/modrinth/modrinthmoddialog.ui \
    src/ui/modrinth/modrinthmodinfowidget.ui \
    src/ui/modrinth/modrinthmoditemwidget.ui \
    src/ui/optifine/optifinemodbrowser.ui \
    src/ui/optifine/optifinemodbrowser.ui \
    src/ui/optifine/optifinemoditemwidget.ui \
    src/ui/replay/replaymodbrowser.ui \
    src/ui/preferences.ui \
    src/ui/replay/replaymodbrowser.ui \
    src/ui/replay/replaymoditemwidget.ui \
    src/ui/tagswidget.ui \
    src/ui/windowstitlebar.ui

TRANSLATIONS += \
    languages/zh_CN.ts \
    languages/zh_TW.ts
CONFIG += lrelease
CONFIG += embed_translations

#cpp-semver: https://github.com/easz/cpp-semver
include(3rdparty/cpp-semver/cpp-semver.pri)

#tomlplusplus: https://github.com/marzer/tomlplusplus
include(3rdparty/tomlplusplus/tomlplusplus.pri)

RESOURCES += \
    src/icons/breeze/breeze-modmanager.qrc \
    src/images/image.qrc \
    src/qss/stylesheets.qrc

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
  desktop.files += package/ModManager.desktop

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
    package/ModManager.desktop \
    src/modmanager.png \
    src/qss/qdarkstyle-light/style.qss

win32: RC_ICONS = package/modmanager.ico

#dependencies
unix {
  message("unix-like build")
  exists(/usr/lib/x86_64-linux-gnu/libquazip5.a) {
    LIBS += -L$$quote(/usr/lib/x86_64-linux-gnu) -lquazip5
  } else {
    LIBS += -lquazip1-qt5
  }
  LIBS += -L$$quote(/usr/local/lib) -laria2
  INCLUDEPATH += \
      /usr/include/QuaZip-Qt5-1.2/quazip \
      /usr/include/QuaZip-Qt5-1.1/quazip \
      /usr/include/quazip

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
  INCLUDEPATH += /usr/local/include/QuaZip-Qt5-1.2/quazip
}

win32 {
  LIBS += -ldwmapi
  contains(QMAKE_HOST.arch, x86_64) {
    message("win32 x86_64 build")
    INCLUDEPATH += C:/msys64/mingw64/include
    INCLUDEPATH += C:/msys64/mingw64/include/QuaZip-Qt5-1.3/quazip
    LIBS += -L$$quote(C:/msys64/mingw64/bin) -laria2-0 -lquazip1-qt5
    #LIBS += "C:/msys64/mingw64/lib/libaria2.dll.a"
    #LIBS += "C:/msys64/mingw64/lib/libquazip1-qt5.dll.a"
  } else {
    message("win32 x86 build")
    INCLUDEPATH += C:/msys64/mingw32/include
    INCLUDEPATH += C:/msys64/mingw32/include/QuaZip-Qt5-1.3/quazip
    LIBS += -L$$quote(C:/msys64/mingw32/bin) -laria2 -lquazip1-qt5
  }
}
