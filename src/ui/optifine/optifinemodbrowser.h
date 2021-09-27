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

    void on_openFolderButton_clicked();

    void on_downloadPathSelect_currentIndexChanged(int index);

    void on_checkBox_stateChanged(int arg1);

    void on_searchText_textEdited(const QString &arg1);

    void on_versionSelect_currentTextChanged(const QString &arg1);

private:
    Ui::OptifineModBrowser *ui;
    OptifineAPI *api_;
    BMCLAPI *bmclapi_;
    LocalModPath *downloadPath_ = nullptr;
    bool isUiSet_ = false;

    void getModList();
};

#endif // OPTIFINEMODBROWSER_H
