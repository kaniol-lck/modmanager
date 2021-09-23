#ifndef OPTIFINEMODBROWSER_H
#define OPTIFINEMODBROWSER_H

#include <QWidget>

class OptifineAPI;
class LocalModPath;

namespace Ui {
class OptifineModBrowser;
}

class OptifineModBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit OptifineModBrowser(QWidget *parent = nullptr);
    ~OptifineModBrowser();

signals:
    void downloadPathChanged(LocalModPath *path);

private slots:
    void updateLocalPathList();

    void on_openFolderButton_clicked();

    void on_downloadPathSelect_currentIndexChanged(int index);

    void on_checkBox_stateChanged(int arg1);

    void on_versionSelect_currentIndexChanged(int index);

    void on_searchText_textEdited(const QString &arg1);

private:
    Ui::OptifineModBrowser *ui;
    OptifineAPI *api_;
    LocalModPath *downloadPath_;
    bool isUiSet_ = false;

    void getModList();
};

#endif // OPTIFINEMODBROWSER_H
