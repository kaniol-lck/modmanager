#ifndef REPLAYMODBROWSER_H
#define REPLAYMODBROWSER_H

#include <QWidget>

#include "local/localmodpathinfo.h"

class LocalModPath;
class ReplayAPI;

namespace Ui {
class ReplayModBrowser;
}

class ReplayModBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit ReplayModBrowser(QWidget *parent = nullptr);
    ~ReplayModBrowser();

public slots:
    void searchModByPathInfo(const LocalModPathInfo &info);

signals:
    void downloadPathChanged(LocalModPath *path);

private slots:
    void updateLocalPathList();
    void filterList();

    void on_openFolderButton_clicked();

    void on_downloadPathSelect_currentIndexChanged(int index);

private:
    Ui::ReplayModBrowser *ui;
    ReplayAPI *api_;
    LocalModPath *downloadPath_ = nullptr;
    bool isUiSet_ = false;

    void getModList();
};

#endif // REPLAYMODBROWSER_H
