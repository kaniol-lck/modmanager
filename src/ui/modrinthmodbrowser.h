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
    void updateVersionList();

    void on_searchButton_clicked();

    void onSliderChanged(int i);

    void on_modListWidget_doubleClicked(const QModelIndex &index);

    void on_sortSelect_currentIndexChanged(int);

    void on_versionSelect_currentIndexChanged(int);

    void on_loaderSelect_currentIndexChanged(int);

private:
    Ui::ModrinthModBrowser *ui;
    QList<ModrinthMod*> modList_;
    QString currentName_;
    int currentIndex_;
    bool isUiSet_ = false;
    bool hasMore_ = false;

    void getModList(QString name, int index = 0);
};

#endif // MODRINTHMODBROWSER_H
