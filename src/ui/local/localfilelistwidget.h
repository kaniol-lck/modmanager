#ifndef LOCALFILELISTWIDGET_H
#define LOCALFILELISTWIDGET_H

#include <QWidget>

class LocalMod;
class QStandardItemModel;

class LocalModFile;
namespace Ui {
class LocalFileListWidget;
}

class LocalFileListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LocalFileListWidget(QWidget *parent = nullptr);
    ~LocalFileListWidget();

    void setMod(LocalMod *mod);
signals:
    void modChanged();
private:
    Ui::LocalFileListWidget *ui;
    QStandardItemModel *model_;
    LocalMod *mod_ = nullptr;
    void addModFile(LocalModFile *file);
};

#endif // LOCALFILELISTWIDGET_H
