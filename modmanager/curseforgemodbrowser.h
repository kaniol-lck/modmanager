#ifndef CURSEFORGEMODBROWSER_H
#define CURSEFORGEMODBROWSER_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QList>

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

private slots:
    void on_searchButton_clicked();
    void downloadFinished(QNetworkReply* reply);

private:
    Ui::CurseforgeModBrowser *ui;
    QNetworkAccessManager *accessManager;
    QList<CurseforgeMod*> modList;

};

#endif // CURSEFORGEMODBROWSER_H
