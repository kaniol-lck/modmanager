#ifndef MODRINTHMODBROWSER_H
#define MODRINTHMODBROWSER_H

#include <QWidget>

#include "modrinth/modrinthmod.h"

namespace Ui {
class ModrinthModBrowser;
}

class ModrinthModBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit ModrinthModBrowser(QWidget *parent = nullptr);
    ~ModrinthModBrowser();

private slots:
    void on_searchButton_clicked();

    void onSliderChanged(int i);

    void on_modListWidget_doubleClicked(const QModelIndex &index);

private:
    Ui::ModrinthModBrowser *ui;
    QList<ModrinthMod*> modList;
    QString currentName;
    int currentIndex;
    bool isUiSet = false;
    bool hasMore = false;

    void getModList(QString name, int index = 0);
};

#endif // MODRINTHMODBROWSER_H
