#ifndef LOCALMODBROWSER_H
#define LOCALMODBROWSER_H

#include <QWidget>
#include <QDir>

#include "moddirinfo.h"
#include "local/localmodinfo.h"

class QNetworkAccessManager;

namespace Ui {
class LocalModBrowser;
}

class LocalModBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit LocalModBrowser(QWidget *parent, QNetworkAccessManager *manager, const ModDirInfo &info);
    ~LocalModBrowser();

    void setModDirInfo(const ModDirInfo &newModDirInfo);

signals:
    void modsDirUpdated();

public slots:
    void updateModList();

private slots:
    void on_modListWidget_currentRowChanged(int currentRow);

private:
    Ui::LocalModBrowser *ui;
    ModDirInfo modDirInfo;
    QList<LocalModInfo> modList;

    QNetworkAccessManager *accessManager;
};

#endif // LOCALMODBROWSER_H
