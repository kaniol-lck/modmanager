QT       += core gui network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17
#DEFINES += QT_NO_VERSION_TAGGING
DEFINES += _LIBCPP_DISABLE_AVAILABILITY

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050F00

INCLUDEPATH += $$PWD/src

SOURCES += \
    $$files(src/*.cpp, true)

HEADERS += \
    $$files(src/*.h, true) \
    $$files(src/*.hpp, true)

FORMS += \
    $$files(src/*.ui, true)

TRANSLATIONS += \
    $$files(languages/*.ts, true)

CONFIG += lrelease
CONFIG += embed_translations

#cpp-semver: https://github.com/easz/cpp-semver
include(3rdparty/cpp-semver/cpp-semver.pri)

#tomlplusplus: https://github.com/marzer/tomlplusplus
include(3rdparty/tomlplusplus/tomlplusplus.pri)

RESOURCES += \
    $$files(src/*.qrc, true)

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
# QMAKE_CXXFLAGS_DEBUG += -g3 -O0
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
unix:!macx {
  message("unix-like build")

  message(HEADERS)
  exists(/usr/include/KF5/KWindowSystem){
    message("under KDE")
    DEFINES += DE_KDE
    LIBS += -lKF5WindowSystem
    INCLUDEPATH += /usr/include/KF5/KWindowSystem
  }

  equals(QT_MAJOR_VERSION, 5): QMAKE_CXXFLAGS += $$system($$pkgConfigExecutable() --cflags libaria2 quazip1-qt5)
  equals(QT_MAJOR_VERSION, 6): QMAKE_CXXFLAGS += $$system($$pkgConfigExecutable() --cflags libaria2 quazip1-qt5)
  # pkg-config works wrongly, help me
  equals(QT_MAJOR_VERSION, 5): LIBS += -L/usr/lib/x86_64-linux-gnu -lquazip1-qt5 -L/usr/local/lib -laria2
  equals(QT_MAJOR_VERSION, 6): LIBS += -L/usr/lib/x86_64-linux-gnu -lquazip1-qt6 -L/usr/local/lib -laria2
  # CONFIG += link_pkgconfig
  # equals(QT_MAJOR_VERSION, 5): PKGCONFIG += libaria2 quazip1-qt5
  # equals(QT_MAJOR_VERSION, 6): PKGCONFIG += libaria2 quazip1-qt6
}

macx {
  message("macos build")
  
  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15
  CONFIG += sdk_no_version_check

  INCLUDEPATH += /usr/local/include
  LIBS += -L/usr/local/lib -llibaria2
  equals(QT_MAJOR_VERSION,5):INCLUDEPATH += /usr/local/include/QuaZip-Qt5-1.4/quazip
  equals(QT_MAJOR_VERSION,6):INCLUDEPATH += /usr/local/include/QuaZip-Qt6-1.4/quazip
  equals(QT_MAJOR_VERSION,5):LIBS += -L/usr/local/lib -lquazip1-qt5
  equals(QT_MAJOR_VERSION,6):LIBS += -L/usr/local/lib -lquazip1-qt6
}

win32 {
  #native blur
  LIBS += -ldwmapi


  QMAKE_CXXFLAGS += $$system($$pkgConfigExecutable() --cflags libaria2)
  LIBS += $$system($$pkgConfigExecutable() --libs libaria2 | sed 's/\/lib\b/\/bin/' | sed 's/-laria2/-laria2-0/')
  equals(QT_MAJOR_VERSION, 5){
    QMAKE_CXXFLAGS += $$system($$pkgConfigExecutable() --cflags quazip1-qt5)
    LIBS += $$system($$pkgConfigExecutable() --libs quazip1-qt5)
  }
  equals(QT_MAJOR_VERSION, 6){
    QMAKE_CXXFLAGS += $$system($$pkgConfigExecutable() --cflags quazip1-qt6)
    LIBS += $$system($$pkgConfigExecutable() --libs quazip1-qt6)
  }
}
