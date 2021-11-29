#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <QStandardPaths>
#include <QDebug>

#include "tag/tagcategory.h"

#define getterAndSetter(name, type, key, defaultValue) \
    void set##name(const decltype(QVariant().to##type()) &key){\
        setValue(#key, key);\
    }\
    \
    decltype(QVariant().to##type()) get##name() const{\
        return value(#key, old_.value(#key, defaultValue)).to##type();/*for compatibility*/\
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
    explicit Config() :
        QSettings(),
        old_("modmanager.ini", QSettings::IniFormat)
    {
        setIniCodec("UTF-8");
        old_.setIniCodec("UTF-8");
    }

    //General
    getterAndSetter(SmoothScroll, Bool, smoothScroll, true)
    getterAndSetter(ScrollSpeed, Double, scrollSpeed, 1.0)
    getterAndSetter(ScrollAcceleration, Double, scrollAcceleration, 1.0)
    getterAndSetter(ScrollFriction, Double, scrollFriction, 1.0)
    getterAndSetter(ShowModAuthors, Bool, showModAuthors, true)
    getterAndSetter(ShowModDateTime, Bool, showModDateTime, true)
    getterAndSetter(ShowModCategory, Bool, showModCategory, true)
    getterAndSetter(ShowModLoaderType, Bool, showModLoaderType, true)
    getterAndSetter(ShowModReleaseType, Bool, showModReleaseType, true)
    getterAndSetter(ShowModGameVersion, Bool, showModGameVersion, true)
    getterAndSetter(AutoTranslate, Bool, autoTranslate, false)

#ifdef DE_KDE
    getterAndSetter(UseSystemIconTheme, Bool, useSystemIconTheme, true)
#else
    getterAndSetter(UseSystemIconTheme, Bool, useSystemIconTheme, false)
#endif

    void setShowTagCategories(const QList<TagCategory> &categories)
    {
        QList<QVariant> list;
        for(auto category : categories)
            list << category.id();
        setValue("showCategories", list);
    }

    QList<TagCategory> getShowTagCategories()
    {
        QList<TagCategory> categories;
        auto &&variant = value("showCategories", true);
        if(variant.toBool()) return TagCategory::PresetCategories;
        for(auto &&category : variant.toList())
            categories << TagCategory::fromId(category.toString());
        return categories;
    }

    //Explore
    getterAndSetter(DownloadPath, String, downloadPath, QStandardPaths::writableLocation(QStandardPaths::DownloadLocation))
    getterAndSetter(SearchResultCount, Int, searchResultCount, 30)
    enum OptifineSourceType{ Official, BMCLAPI };
    getterAndSetter(OptifineSource, Int, optifineSource, Official)
    getterAndSetter(ShowModrinthSnapshot, Bool, showModrinthSnapshot, false)
    getterAndSetter(ShowCurseforge, Bool, ShowCurseforge, true)
    getterAndSetter(ShowModrinth, Bool, showModrinth, true)
    getterAndSetter(ShowOptiFine, Bool, showOptiFine, true)
    getterAndSetter(ShowReplayMod, Bool, showReplayMod, true)

    //Local
    getterAndSetter(CommonPath, String, commonPath, "")
    enum RightClickTagMenuType{ AllAvailable, CurrentPath };
    getterAndSetter(RightClickTagMenu, Int, rightClickTagMenu, AllAvailable)
    getterAndSetter(LoadModsOnStartup, Bool, loadModsOnStartup, false)
    getterAndSetter(StarredAtTop, Bool, starredAtTop, true)
    getterAndSetter(DisabedAtBottom, Bool, disabedAtBottom, true)
    enum VersionMatchType{ MinorVersion, MajorVersion };
    getterAndSetter(VersionMatch, Int, versionMatch, MinorVersion);
    enum LoaderMatchType{ ExactMatch, IncludeUnmarked };
    getterAndSetter(LoaderMatch, Int, loaderMatch, IncludeUnmarked);
    getterAndSetter(UseCurseforgeUpdate, Bool, useCurseforgeUpdate, true)
    getterAndSetter(UseBetaUpdate, Bool, useBetaUpdate, true)
    getterAndSetter(UseAlphaUpdate, Bool, useAlphaUpdate, true)
    getterAndSetter(UseModrinthUpdate, Bool, useModrinthUpdate, true)
    enum PostUpdateType{ Delete, Keep, DoNothing };
    getterAndSetter(PostUpdate, Int, postUpdate, Keep)
    enum UpdateCheckIntervalType{ Always, EveryDay, Never };
    getterAndSetter(UpdateCheckInterval, Int, updateCheckIntervalType, EveryDay)

    //Network
    getterAndSetter(Aria2timeout, Int, aria2timeout, 60)
    getterAndSetter(Aria2maxTries, Int, aria2maxTries, 5)
    getterAndSetter(Aria2maxConcurrentDownloads, Int, aria2maxConcurrentDownloads, 5)
    getterAndSetter(NetworkRequestTimeout, Int, networkRequestTimeout, 60000)

    //Path List
    getterAndSetter(LocalPathList, List, localPathList, QVariant())
    //Download List
    getterAndSetter(DownloaderList, List, downloaderList, QVariant())

    //others
    getterAndSetter(ModrinthMultiVersion, Bool, modrinthMultiVersion, false)
    getterAndSetter(ModrinthMultiCategory, Bool, modrinthMultiCategory, false)
    getterAndSetter(LockPanel, Bool, LockPanel, true)
    getterAndSetter(Geometry, ByteArray, geometry, QByteArray())
    getterAndSetter(WindowState, ByteArray, windowState, QByteArray())
#if defined (DE_KDE)
    getterAndSetter(EnableBlurBehind, Bool, enableBlurBehind, true)
#else
    getterAndSetter(EnableBlurBehind, Bool, enableBlurBehind, false)
#endif
    getterAndSetter(UseFramelessWindow, Bool, useFramelessWindow, false)
    getterAndSetter(CustomStyle, String, customStyle, "light")
    getterAndSetter(LocalModsHeaderSections, List, localModsHeaderSections, QVariant())
private:
    QSettings old_;
};

#undef getterAndSetter
#undef getterAndSetter_prefix
#endif // CONFIG_H
