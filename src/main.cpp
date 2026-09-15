#include "qss/stylesheets.h"
#include "ui/modmanager.h"

#include <QApplication>
#include <QLocale>
#include <QSettings>
#include <QTranslator>

#include "ui/framelesswrapper.h"
#include "config.hpp"

#include <ui/windowstitlebar.h>

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    //先设定组织名/应用名，下面的 QSettings 才能定位到正确的配置位置
    QCoreApplication::setOrganizationName(QStringLiteral("kaniol"));
    QCoreApplication::setApplicationName(QStringLiteral("Mod Manager"));

    //这两个属性必须在 QApplication 构造之前设置，写在构造之后就完全不生效。
    //它们的作用是阻止 winId() 把窗口 native 化（FramelessWrapper 会调用 winId() 设模糊），
    //native 化会连带影响事件分发与绘制。只在开启无边框时设置，避免影响其它模式的窗口行为。
    if(QSettings().value(QStringLiteral("useFramelessWindow"), false).toBool()){
        QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
        QCoreApplication::setAttribute(Qt::AA_NativeWindows, false);
    }
#endif

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/image/modmanager.png"));
    a.setOrganizationName("kaniol");
    a.setApplicationName("Mod Manager");
    a.setApplicationDisplayName(QObject::tr("Mod Manager"));
    a.setStyleSheet(styleSheetPath(Config().getCustomStyle()));

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
    //基准字号。QSS 中显式指定的字号按 9(次要信息) / 10(正文) / 11(小标题) / 13(主标题)
    //这条阶梯取值，基准取 10pt 才与阶梯一致
    //（原来基准是 9pt，会让正文比 .Description 的 10pt 还小，层级反了）。
    QFont font("微软雅黑", 10);
    a.setFont(font);
#endif
#ifdef Q_OS_MACOS
    QFont font("PingFang", 12);
    a.setFont(font);
#endif
    ModManager w;

#ifdef Q_OS_WIN
    if(Config().getUseFramelessWindow()){
        auto frameless = new FramelessWrapper(&w);
        QObject::connect(&w, &ModManager::closed, frameless, &FramelessWrapper::close);
        frameless->show();
    } else
#endif
        w.show();
    return a.exec();
}
