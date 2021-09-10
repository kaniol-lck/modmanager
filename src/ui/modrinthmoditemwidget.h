#ifndef MODRINTHMODITEMWIDGET_H
#define MODRINTHMODITEMWIDGET_H

#include <QWidget>

#include "modrinth/modrinthfileinfo.h"

class ModrinthMod;

namespace Ui {
class ModrinthModItemWidget;
}

class ModrinthModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModrinthModItemWidget(QWidget *parent, ModrinthMod *mod, const QString &path);
    ~ModrinthModItemWidget();

public slots:
    void setDownloadPath(const QString &newDownloadPath);

private slots:
    void updateIcon();
    void updateFileList();

    void downloadFile(const ModrinthFileInfo &fileInfo);

private:
    Ui::ModrinthModItemWidget *ui;
    ModrinthMod *mod_;
    QString downloadPath_;
};

#endif // MODRINTHMODITEMWIDGET_H
