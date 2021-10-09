#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>

#define getterAndSetter(name, type, key, defaultValue) \
    void set##name(const decltype(QVariant().to##type()) &key){\
        setValue(#key, key);\
    }\
    \
    decltype(QVariant().to##type()) get##name() const{\
        return value(#key, defaultValue).to##type();\
    }

#define getterAndSetter_prefix(name, type, key, prefix, defaultValue) \
    void set##name(const decltype(QVariant().to##type()) &key){\
        setValue(QString(prefix) + "/" + #key, key);\
    }\
    \
    decltype(QVariant().to##type()) get##name() const{\
        return value(QString(prefix) + "/" + #key, defaultValue).to##type();\
    }

class Config : private QSettings
{
public:
    explicit Config();

    //General
    getterAndSetter(DownloadPath, String, downloadPath, "")
    getterAndSetter(CommonPath, String, commonPath, "")
    getterAndSetter(SearchResultCount, Int, searchResultCount, 30)
    enum VersionMatchType{ MinorVersion, MajorVersion };
    getterAndSetter(VersionMatch, Int, versionMatch, MinorVersion);
    getterAndSetter(UseCurseforgeUpdate, Bool, useCurseforgeUpdate, true)
    getterAndSetter(UseModrinthUpdate, Bool, useModrinthUpdate, true)
    enum PostUpdateType{ Delete, Keep, DoNothing };
    getterAndSetter(PostUpdate, Int, postUpdate, Keep)
    enum UpdateCheckIntervalType{ Always, EveryDay, Never };
    getterAndSetter(UpdateCheckInterval, Int, updateCheckIntervalType, EveryDay)
    enum OptifineSourceType{ Official, BMCLAPI };
    getterAndSetter(OptifineSource, Int, optifineSource, Official)
    getterAndSetter(ShowModrinthSnapshot, Bool, showModrinthSnapshot, false)

    //Local
    enum RightClickTagMenuType{ AllAvailable, CurrentPath };
    getterAndSetter(RightClickTagMenu, Int, rightClickTagMenu, AllAvailable)
    getterAndSetter(StarredAtTop, Bool, starredAtTop, true)
    getterAndSetter(DisabedAtBottom, Bool, disabedAtBottom, true)

    //Network
    getterAndSetter(ThreadCount, Int, threadCount, 8)
    getterAndSetter(DownloadCount, Int, downloadCount, 16)

    //Path List
    getterAndSetter(LocalPathList, List, localPathList, QVariant())
    //Download List
    getterAndSetter(DownloaderList, List, downloaderList, QVariant())

    //others
    getterAndSetter(TabSelectBarArea, Int, tabSelectBarArea, Qt::LeftToolBarArea)
    getterAndSetter(MainWindowWidth, Int, mainWindowWidth, 1440);
    getterAndSetter(MainWindowHeight, Int, mainWindowHeight, 900);
};

#endif // CONFIG_H
