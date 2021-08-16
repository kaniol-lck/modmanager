#ifndef CURSEFORGEMODBROWSER_H
#define CURSEFORGEMODBROWSER_H

#include <QWidget>
#include <QList>

class QNetworkReply;
class CurseforgeMod;

namespace Ui {
class CurseforgeModBrowser;
}

class CurseforgeModBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit CurseforgeModBrowser(QWidget *parent = nullptr);
    ~CurseforgeModBrowser();

public slots:
    void updateVersions();

private slots:
    void on_searchButton_clicked();
    void downloadFinished(QNetworkReply* reply);
    void onSliderChanged(int i);

    void on_modListWidget_doubleClicked(const QModelIndex &index);

    void on_versionSelect_currentIndexChanged(int);

    void on_sortSelect_currentIndexChanged(int);

private:
    Ui::CurseforgeModBrowser *ui;
    QList<CurseforgeMod*> modList;
    QString currentName;
    int currentIndex;

    void getModList(QString name, int index = 0);

};

#endif // CURSEFORGEMODBROWSER_H
