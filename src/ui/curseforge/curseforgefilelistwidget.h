#ifndef CURSEFORGEFILELISTWIDGET_H
#define CURSEFORGEFILELISTWIDGET_H

#include <QWidget>

class CurseforgeMod;
class CurseforgeModBrowser;
class LocalModPath;
class QStandardItemModel;
class DownloadPathSelectMenu;
class LocalMod;
namespace Ui {
class CurseforgeFileListWidget;
}

class CurseforgeFileListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeFileListWidget(CurseforgeModBrowser *parent);
    explicit CurseforgeFileListWidget(QWidget *parent = nullptr, LocalMod *localMod = nullptr);
    ~CurseforgeFileListWidget();

    void setMod(CurseforgeMod *mod);
    DownloadPathSelectMenu *downloadPathSelectMenu() const;

    void setLocalMod(LocalMod *newLocalMod);
    void setDownloadPathSelectMenu(DownloadPathSelectMenu *newDownloadPathSelectMenu);

signals:
    void modChanged();

public slots:
    void updateUi();

private slots:
    void updateFileList();
    void updateIndexWidget();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::CurseforgeFileListWidget *ui;
    DownloadPathSelectMenu *downloadPathSelectMenu_ = nullptr;
    QStandardItemModel *model_;
    CurseforgeMod *mod_ = nullptr;
    LocalMod *localMod_ = nullptr;
};

#endif // CURSEFORGEFILELISTWIDGET_H
