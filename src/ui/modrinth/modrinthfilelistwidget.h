#ifndef MODRINTHFILELISTWIDGET_H
#define MODRINTHFILELISTWIDGET_H

#include <QStandardItemModel>
#include <QWidget>

class ModrinthMod;
class ModrinthModBrowser;
class LocalModPath;
class DownloadPathSelectMenu;

class LocalMod;
namespace Ui {
class ModrinthFileListWidget;
}

class ModrinthFileListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModrinthFileListWidget(ModrinthModBrowser *parent);
    explicit ModrinthFileListWidget(QWidget *parent = nullptr, LocalMod *localMod = nullptr);
    ~ModrinthFileListWidget();

    void setMod(ModrinthMod *mod);
    DownloadPathSelectMenu *downloadPathSelectMenu() const;

    void setLocalMod(LocalMod *newLocalMod);
    void setDownloadPathSelectMenu(DownloadPathSelectMenu *newDownloadPathSelectMenu);

signals:
    void modChanged();

public slots:
    void updateUi();

private slots:
    void updateFullInfo();
    void updateFileList();
    void updateIndexWidget();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::ModrinthFileListWidget *ui;
    DownloadPathSelectMenu *downloadPathSelectMenu_ = nullptr;
    QStandardItemModel model_;
    ModrinthMod *mod_ = nullptr;
    LocalMod *localMod_ = nullptr;
};

#endif // MODRINTHFILELISTWIDGET_H
