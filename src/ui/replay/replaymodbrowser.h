#ifndef REPLAYMODBROWSER_H
#define REPLAYMODBROWSER_H

#include "ui/explorebrowser.h"

class LocalModPath;
class ReplayAPI;


class QStandardItemModel;
namespace Ui {
class ReplayModBrowser;
}

class ReplayModBrowser : public ExploreBrowser
{
    Q_OBJECT

public:
    explicit ReplayModBrowser(QWidget *parent = nullptr);
    ~ReplayModBrowser();

public slots:
    void refresh() override;
    void searchModByPathInfo(const LocalModPathInfo &info) override;
    void updateUi() override;

signals:
    void downloadPathChanged(LocalModPath *path);

private slots:
    void updateLocalPathList();
    void filterList();
    void on_openFolderButton_clicked();
    void on_downloadPathSelect_currentIndexChanged(int index);
    void updateIndexWidget();
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    Ui::ReplayModBrowser *ui;
    QStandardItemModel *model_;
    ReplayAPI *api_;
    LocalModPath *downloadPath_ = nullptr;
    bool isUiSet_ = false;
    bool inited_ = false;

    void getModList();
};

#endif // REPLAYMODBROWSER_H
