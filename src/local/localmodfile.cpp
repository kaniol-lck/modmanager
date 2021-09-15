#include "localmodfile.h"

#include <QDir>

const QStringList LocalModFile::availableSuffix{ "jar", "old", "disabled"};

LocalModFile::LocalModFile(QObject *parent, const QString &path) :
    QObject(parent),
    path_(path),
    fileInfo_(path)
{

}

bool LocalModFile::rename(const QString newBaseName)
{
    auto [ baseName, suffix ] = baseNameFullSuffix();
    QFile file(path_);
    auto newPath = QDir(fileInfo_.absolutePath()).absoluteFilePath(newBaseName + suffix);
    if(file.rename(newPath)){
        path_ = newPath;
        fileInfo_.setFile(path_);
            emit fileChanged();
        return true;
    } else
    return false;
}

bool LocalModFile::addOld()
{
    path_.append(".old");
    fileInfo_.setFile(path_);

    QFile file(path_);
    bool bl = file.rename(file.fileName().append(".old"));
    if(bl)
        emit fileChanged();
    return bl;
}

bool LocalModFile::removeOld()
{
    path_.remove(".old");
    fileInfo_.setFile(path_);

    QFile file(path_);
    bool bl = file.rename(file.fileName().remove(".old"));
    if(bl)
        emit fileChanged();
    return bl;
}

std::tuple<QString, QString> LocalModFile::baseNameFullSuffix() const
{
    auto fileName = fileInfo_.fileName();
    QFileInfo fileInfo(fileName);
    QString fullSuffix;
    while(true){
        fileInfo.setFile(fileName);
        auto suffix = fileInfo.suffix();
        if(!availableSuffix.contains(suffix))
            break;
        fileName = fileInfo.completeBaseName();
        fullSuffix += "." + suffix;
    }
    return { fileName, fullSuffix };
}

