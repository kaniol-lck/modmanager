#ifndef LOCALMODBROWSER_H
#define LOCALMODBROWSER_H

#include <QWidget>

#include "local/localmod.h"

namespace Ui {
class LocalModBrowser;
}

class LocalModPath;

class LocalModBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit LocalModBrowser(QWidget *parent, LocalModPath *modPath);
    ~LocalModBrowser();

signals:

public slots:
    void updateModList();

private slots:
    void on_modListWidget_doubleClicked(const QModelIndex &index);

    void on_updateAllButton_clicked();

    void on_searchText_textEdited(const QString &arg1);

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::LocalModBrowser *ui;
    LocalModPath *modPath_;
};

#endif // LOCALMODBROWSER_H
