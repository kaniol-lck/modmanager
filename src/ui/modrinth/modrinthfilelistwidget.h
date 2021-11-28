#ifndef MODRINTHFILELISTWIDGET_H
#define MODRINTHFILELISTWIDGET_H

#include <QWidget>

class ModrinthMod;
class LocalModPath;
class QStandardItemModel;
namespace Ui {
class ModrinthFileListWidget;
}

class ModrinthFileListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModrinthFileListWidget(QWidget *parent = nullptr);
    ~ModrinthFileListWidget();

    void setMod(ModrinthMod *mod);
signals:
    void modChanged();
    void downloadPathChanged(LocalModPath *path);
public slots:
    void setDownloadPath(LocalModPath *newDownloadPath);
    void updateUi();
private slots:
    void updateFullInfo();
    void updateFileList();
    void updateIndexWidget();
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    Ui::ModrinthFileListWidget *ui;
    QStandardItemModel *model_;
    ModrinthMod *mod_ = nullptr;
    LocalModPath *downloadPath_ = nullptr;
};

#endif // MODRINTHFILELISTWIDGET_H
