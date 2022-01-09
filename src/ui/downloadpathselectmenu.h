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

private slots:
    void onAboutToShow();

private:
    QMenu menu_;
    LocalModPath *downloadPath_ = nullptr;
};

#endif // DOWNLOADPATHSELECTMENU_H
