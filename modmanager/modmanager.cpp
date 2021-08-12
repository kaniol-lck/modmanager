#include "modmanager.h"
#include "ui_modmanager.h"

#include "quazip.h"
#include "quazipfile.h"
#include "MurmurHash2.h"

#include <QDebug>
#include <QDir>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QCryptographicHash>

ModManager::ModManager(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ModManager)
{
    ui->setupUi(this);
    auto modDir = QDir("/run/media/kaniol/SanDisk/Minecraft/1.17.1/.minecraft/mods/");
    for (auto entryInfo : modDir.entryInfoList(QDir::Files)) {
        auto filePath = entryInfo.absoluteFilePath();
        qDebug() << filePath;
        QuaZip zipR(filePath);

        //jar file
        QFile jarFile(filePath);
        if(!jarFile.open(QIODevice::ReadOnly)) continue;
        auto jarContent = jarFile.readAll();
        jarFile.close();

        //unzip
        zipR.open(QuaZip::mdUnzip);
        zipR.setCurrentFile("fabric.mod.json");
        QuaZipFile fileR(&zipR);
        if(!fileR.open(QIODevice::ReadOnly)) continue;
        auto json = fileR.readAll();
        fileR.close();
        zipR.close();
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(json, &error);
        if (error.error == QJsonParseError::NoError) {
            if (jsonDocument.isObject()) {
                QVariantMap result = jsonDocument.toVariant().toMap();
                qDebug() << "id:" << result["id"].toString();
                qDebug() << "version:" << result["version"].toString();
                qDebug() << "name:" << result["name"].toString();
                qDebug() << "description:" << result["name"].toString();
                qDebug() << "sha1:" << QCryptographicHash::hash(jarContent, QCryptographicHash::Sha1).toHex();
                qDebug() << "murmurhash:" << QByteArray::number(MurmurHash2(jarContent.data(), jarContent.length(), 1));
            }
        } else {
            qFatal("%s", error.errorString().toUtf8().constData());
            exit(1);
        }
    }
}

ModManager::~ModManager()
{
    delete ui;
}

