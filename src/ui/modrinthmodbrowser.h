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

public slots:
    void updateVersions();

private slots:
    void on_searchButton_clicked();

    void onSliderChanged(int i);

    void on_modListWidget_doubleClicked(const QModelIndex &index);

    void on_sortSelect_currentIndexChanged(int);

private:
    Ui::ModrinthModBrowser *ui;
    QList<ModrinthMod*> modList_;
    QString currentName_;
    int currentIndex_;
    bool isUiSet_ = true;
    bool hasMore_ = false;

    void getModList(QString name, int index = 0);
};

#endif // MODRINTHMODBROWSER_H
