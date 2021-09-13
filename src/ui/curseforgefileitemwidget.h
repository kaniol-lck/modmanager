#ifndef CURSEFORGEFILEITEMWIDGET_H
#define CURSEFORGEFILEITEMWIDGET_H

#include <QWidget>

#include "curseforge/curseforgefileinfo.h"

class CurseforgeMod;
class LocalMod;

namespace Ui {
class CurseforgeFileItemWidget;
}

class CurseforgeFileItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeFileItemWidget(QWidget *parent, CurseforgeMod *mod, const CurseforgeFileInfo &info, const QString &path, LocalMod* localMod = nullptr);
    ~CurseforgeFileItemWidget();

public slots:
    void setDownloadPath(const QString &newDownloadPath);

private slots:
    void on_downloadButton_clicked();

private:
    Ui::CurseforgeFileItemWidget *ui;
    CurseforgeMod *mod_;
    LocalMod *localMod_ = nullptr;
    CurseforgeFileInfo fileInfo_;

    QString downloadPath_;
};

#endif // CURSEFORGEFILEITEMWIDGET_H
