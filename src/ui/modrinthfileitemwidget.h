#ifndef MODRINTHFILEITEMWIDGET_H
#define MODRINTHFILEITEMWIDGET_H

#include <QWidget>

#include "modrinth/modrinthfileinfo.h"

class ModrinthMod;
class LocalMod;

namespace Ui {
class ModrinthFileItemWidget;
}

class ModrinthFileItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModrinthFileItemWidget(QWidget *parent, ModrinthMod *mod, const ModrinthFileInfo &info, const QString &path, LocalMod* localMod = nullptr);
    ~ModrinthFileItemWidget();

public slots:
    void setDownloadPath(const QString &newDownloadPath);

private slots:
    void on_downloadButton_clicked();

private:
    Ui::ModrinthFileItemWidget *ui;
    ModrinthMod *mod_;
    LocalMod *localMod_ = nullptr;
    ModrinthFileInfo fileInfo_;

    QString downloadPath_;
};

#endif // MODRINTHFILEITEMWIDGET_H
