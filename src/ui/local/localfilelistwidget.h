#ifndef LOCALFILELISTWIDGET_H
#define LOCALFILELISTWIDGET_H

#include <QWidget>

class LocalMod;
class QStandardItemModel;

class LocalModFile;

class QButtonGroup;
namespace Ui {
class LocalFileListWidget;
}

class LocalFileListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LocalFileListWidget(QWidget *parent = nullptr);
    ~LocalFileListWidget();

    void setMods(QList<LocalMod *> mods);
signals:
    void modChanged();
private slots:
    void idClicked(int id);
    void onModFileUpdated();
    void updateIndexWidget();
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    Ui::LocalFileListWidget *ui;
    QStandardItemModel *model_;
    QList<LocalMod *> mods_;
    LocalMod *mod_ = nullptr;
    QButtonGroup *buttons_;
    void addModFile(LocalModFile *file, int id, bool checked = false);
};

#endif // LOCALFILELISTWIDGET_H
