#ifndef GITHUBRELEASEINFOWIDGET_H
#define GITHUBRELEASEINFOWIDGET_H

#include <QWidget>


class GitHubRelease;
namespace Ui {
class GitHubReleaseInfoWidget;
}

class GitHubReleaseInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GitHubReleaseInfoWidget(QWidget *parent = nullptr);
    ~GitHubReleaseInfoWidget();

    void setRelease(GitHubRelease *release);

signals:
    void releaseChanged();

private:
    Ui::GitHubReleaseInfoWidget *ui;
    GitHubRelease *release_ = nullptr;
};

#endif // GITHUBRELEASEINFOWIDGET_H
