#ifndef OPTIFINEMODBROWSER_H
#define OPTIFINEMODBROWSER_H

#include <QWidget>

class OptifineAPI;
class BMCLAPI;
class LocalModPath;

#include "local/localmodpathinfo.h"

namespace Ui {
class OptifineModBrowser;
}

class OptifineModBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit OptifineModBrowser(QWidget *parent = nullptr);
    ~OptifineModBrowser();

public slots:
    void searchModByPathInfo(const LocalModPathInfo &info);

signals:
    void downloadPathChanged(LocalModPath *path);

private slots:
    void updateLocalPathList();
    void filterList();

    void on_openFolderButton_clicked();

    void on_downloadPathSelect_currentIndexChanged(int index);

    void on_getOptiFabric_clicked();

    void on_getOptiForge_clicked();

private:
    Ui::OptifineModBrowser *ui;
    OptifineAPI *api_;
    BMCLAPI *bmclapi_;
    LocalModPath *downloadPath_ = nullptr;
    bool isUiSet_ = false;

    void getModList();
};

#endif // OPTIFINEMODBROWSER_H
