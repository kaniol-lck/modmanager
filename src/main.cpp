#include "ui/modmanager.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //setup translator
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    //setup theme
#ifndef Q_OS_UNIX
    QIcon::setThemeName("breeze-modmanager");
#endif
#ifdef Q_OS_WIN
    QFont font("微软雅黑", 10);
    a.setFont(font);
#endif
#ifdef Q_OS_MACOS
    QFont font("PingFang", 12);
    a.setFont(font);
#endif
    a.setWindowIcon(QIcon(":/image/modmanager.png"));
    a.setOrganizationName("kaniol");
    a.setApplicationName("Mod Manager");
    ModManager w;
    w.show();
    return a.exec();
}
