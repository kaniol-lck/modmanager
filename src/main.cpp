#include "ui/modmanager.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "ui/framelesswrapper.h"
#include "config.hpp"

#include <ui/windowstitlebar.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, false);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/image/modmanager.png"));
    a.setOrganizationName("kaniol");
    a.setApplicationName("Mod Manager");

    //setup translator
    QTranslator translator;
    if(auto language = Config().getLanguage();
            !language.isEmpty() && translator.load(":/i18n/" + language)){
        a.installTranslator(&translator);
    } else if(language != "en_US"){
        const QStringList uiLanguages = QLocale::system().uiLanguages();
        for (const QString &locale : uiLanguages) {
            const QString baseName = QLocale(locale).name();
            if (translator.load(":/i18n/" + baseName)) {
                a.installTranslator(&translator);
                break;
            }
        }
    }

    //setup theme
    if(!Config().getUseSystemIconTheme())
        QIcon::setThemeName("breeze-modmanager");

#ifdef Q_OS_WIN
    QFont font("微软雅黑", 9);
    a.setFont(font);
#endif
#ifdef Q_OS_MACOS
    QFont font("PingFang", 12);
    a.setFont(font);
#endif
    ModManager w;

#ifdef Q_OS_WIN
    if(Config().getUseFramelessWindow()){
        auto titleBar = new WindowsTitleBar(&w, w.windowTitle(), w.menuBar());
        QObject::connect(&w, &ModManager::menuBarChanged, titleBar, &WindowsTitleBar::updateMenuBar);
        auto frameless = new FramelessWrapper(nullptr, &w, titleBar);
        QObject::connect(&w, &ModManager::closed, frameless, &FramelessWrapper::close);
        titleBar->setParentWidget(frameless);
        frameless->show();
    } else
#endif
        w.show();
    return a.exec();
}
