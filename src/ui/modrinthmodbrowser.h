#ifndef MODRINTHMODBROWSER_H
#define MODRINTHMODBROWSER_H

#include <QWidget>

#include "local/localmodpathinfo.h"

class ModrinthAPI;
class ModrinthMod;

namespace Ui {
class ModrinthModBrowser;
}

class ModrinthModBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit ModrinthModBrowser(QWidget *parent = nullptr);
    ~ModrinthModBrowser();

public slots:
    void searchModByPathInfo(const LocalModPathInfo &info);

private slots:
    void updateVersionList();

    void updateLocalPathList();

    void on_searchButton_clicked();

    void onSliderChanged(int i);

    void on_modListWidget_doubleClicked(const QModelIndex &index);

    void on_sortSelect_currentIndexChanged(int);

    void on_versionSelect_currentIndexChanged(int);

    void on_loaderSelect_currentIndexChanged(int);

    void on_openFolderButton_clicked();

private:
    Ui::ModrinthModBrowser *ui;
    ModrinthAPI *api_;
    QList<ModrinthMod*> modList_;
    QStringList downloadPathList_;
    QString currentName_;
    int currentIndex_;
    bool isUiSet_ = false;
    bool hasMore_ = false;

    void getModList(QString name, int index = 0);
};

#endif // MODRINTHMODBROWSER_H
