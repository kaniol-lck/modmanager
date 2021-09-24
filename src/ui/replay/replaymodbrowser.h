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

    void on_openFolderButton_clicked();

    void on_downloadPathSelect_currentIndexChanged(int index);

    void on_searchText_textEdited(const QString &arg1);

    void on_versionSelect_currentTextChanged(const QString &arg1);

private:
    Ui::ReplayModBrowser *ui;
    ReplayAPI *api_;
    LocalModPath *downloadPath_ = nullptr;
    bool isUiSet_ = false;

    void getModList();
};

#endif // REPLAYMODBROWSER_H
