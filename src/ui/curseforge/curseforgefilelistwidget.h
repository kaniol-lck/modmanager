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
    explicit CurseforgeFileListWidget(QWidget *parent, LocalMod *localMod);
    explicit CurseforgeFileListWidget(QWidget *parent = nullptr);
    ~CurseforgeFileListWidget();

    void setMod(CurseforgeMod *mod);
    void setBrowser(CurseforgeModBrowser *newBrowser);
    DownloadPathSelectMenu *downloadPathSelectMenu() const;

    void setLocalMod(LocalMod *newLocalMod);

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
    CurseforgeModBrowser *browser_ = nullptr;
    DownloadPathSelectMenu *downloadPathSelectMenu_ = nullptr;
    QStandardItemModel *model_;
    CurseforgeMod *mod_ = nullptr;
    LocalMod *localMod_ = nullptr;
};

#endif // CURSEFORGEFILELISTWIDGET_H
