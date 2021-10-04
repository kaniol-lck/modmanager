#ifndef MODRINTHMODITEMWIDGET_H
#define MODRINTHMODITEMWIDGET_H

#include <QWidget>

#include "modrinth/modrinthfileinfo.h"

class ModrinthMod;
class LocalModPath;

namespace Ui {
class ModrinthModItemWidget;
}

class ModrinthModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModrinthModItemWidget(QWidget *parent, ModrinthMod *mod);
    ~ModrinthModItemWidget();

    ModrinthMod *mod() const;

public slots:
    void setDownloadPath(LocalModPath *newDownloadPath);

private slots:
    void updateIcon();
    void updateFileList();

    void downloadFile(const ModrinthFileInfo &fileInfo);

private:
    Ui::ModrinthModItemWidget *ui;
    QList<QWidget *> tagWidgets_;
    ModrinthMod *mod_;
    LocalModPath *downloadPath_;
};

#endif // MODRINTHMODITEMWIDGET_H
