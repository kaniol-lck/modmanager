#ifndef MODRINTHFILELISTWIDGET_H
#define MODRINTHFILELISTWIDGET_H

#include <QWidget>

class ModrinthMod;
class ModrinthModBrowser;
class LocalModPath;
class QStandardItemModel;
class DownloadPathSelectMenu;
namespace Ui {
class ModrinthFileListWidget;
}

class ModrinthFileListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModrinthFileListWidget(ModrinthModBrowser *parent);
    explicit ModrinthFileListWidget(QWidget *parent = nullptr);
    ~ModrinthFileListWidget();

    void setMod(ModrinthMod *mod);
    DownloadPathSelectMenu *downloadPathSelectMenu() const;

    void setBrowser(ModrinthModBrowser *newBrowser);

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
    ModrinthModBrowser *browser_ = nullptr;
    DownloadPathSelectMenu *downloadPathSelectMenu_ = nullptr;
    QStandardItemModel *model_;
    ModrinthMod *mod_ = nullptr;
};

#endif // MODRINTHFILELISTWIDGET_H
