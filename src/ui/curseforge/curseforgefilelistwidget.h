#ifndef CURSEFORGEFILELISTWIDGET_H
#define CURSEFORGEFILELISTWIDGET_H

#include <QWidget>

class CurseforgeMod;
class LocalModPath;
class QStandardItemModel;
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
    void onSliderChanged(int i);
private:
    Ui::CurseforgeFileListWidget *ui;
    QStandardItemModel *model_;
    CurseforgeMod *mod_ = nullptr;
    LocalModPath *downloadPath_ = nullptr;
    static constexpr int kLoadSize = 10;
};

#endif // CURSEFORGEFILELISTWIDGET_H
