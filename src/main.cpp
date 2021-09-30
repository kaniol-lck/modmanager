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
#ifdef Q_OS_WIN
    QIcon::setThemeName("breeze-modmanager");
    QFont font("微软雅黑", 12);
    a.setFont(font);
#endif
    qApp->setWindowIcon(QIcon(":/image/modmanager.png"));
    ModManager w;
    w.show();
    return a.exec();
}
