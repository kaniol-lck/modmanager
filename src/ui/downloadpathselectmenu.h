#ifndef DOWNLOADPATHSELECTMENU_H
#define DOWNLOADPATHSELECTMENU_H

#include <QMenu>

class LocalModPath;
class DownloadPathSelectMenu : public QMenu
{
    Q_OBJECT
public:
    explicit DownloadPathSelectMenu(QWidget *parent = nullptr);

    LocalModPath *downloadPath() const;
    void setDownloadPath(LocalModPath *newDownloadPath);

signals:
    void DownloadPathChanged();

private slots:
    void onAboutToShow();

private:
    LocalModPath *downloadPath_ = nullptr;
};

#endif // DOWNLOADPATHSELECTMENU_H
