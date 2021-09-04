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

    getterAndSetter(AutoCheckUpdate, Bool, autoCheckUpdate, true)
    getterAndSetter(DeleteOld, Bool, deleteOld, true)

    getterAndSetter(ThreadCount, Int, threadCount, 8)
    getterAndSetter(DownloadCount, Int, downloadCount, 8)

    getterAndSetter(DirList, List, dirList, QVariant())

};

#endif // CONFIG_H
