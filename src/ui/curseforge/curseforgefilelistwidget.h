#ifndef CURSEFORGEFILELISTWIDGET_H
#define CURSEFORGEFILELISTWIDGET_H

#include <QWidget>

class CurseforgeMod;
class LocalModPath;
namespace Ui {
class CurseforgeFileListWidget;
}

class CurseforgeFileListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeFileListWidget(QWidget *parent = nullptr);
    ~CurseforgeFileListWidget();

    void setMod(CurseforgeMod *mod);
signals:
    void modChanged();
    void downloadPathChanged(LocalModPath *path);
public slots:
    void setDownloadPath(LocalModPath *newDownloadPath);
private slots:
    void updateFileList();
private:
    Ui::CurseforgeFileListWidget *ui;
    CurseforgeMod *mod_ = nullptr;
    LocalModPath *downloadPath_ = nullptr;
};

#endif // CURSEFORGEFILELISTWIDGET_H
