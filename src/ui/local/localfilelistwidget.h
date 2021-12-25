#ifndef LOCALFILELISTWIDGET_H
#define LOCALFILELISTWIDGET_H

#include <QWidget>

class LocalMod;
class QStandardItemModel;

class LocalModFile;
class LocalModPath;
class QButtonGroup;
namespace Ui {
class LocalFileListWidget;
}

class LocalFileListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LocalFileListWidget(QWidget *parent = nullptr, LocalModPath *path = nullptr);
    ~LocalFileListWidget();

    void setMods(QList<LocalMod *> mods);
signals:
    void modChanged();
private slots:
    void idClicked(int id);
    void onModFileUpdated();
    void onUpdatesReady();
    void updateIndexWidget();
    void updateUpdateIndexWidget();
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    LocalModPath *path_ = nullptr;
    Ui::LocalFileListWidget *ui;
    QStandardItemModel *updateModel_;
    QStandardItemModel *model_;
    QList<LocalMod *> mods_;
    LocalMod *mod_ = nullptr;
    QButtonGroup *buttons_;
    void addModFile(LocalModFile *file, int id, bool checked = false);
};

#endif // LOCALFILELISTWIDGET_H
