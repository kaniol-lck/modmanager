#ifndef GITHUBREPOINFO_H
#define GITHUBREPOINFO_H

#include <QIcon>
#include <QUrl>

class GitHubRepoInfo
{
public:
    GitHubRepoInfo() = default;
    GitHubRepoInfo(const QString &user, const QString &repo, const QString &name = QString(), const QIcon &icon = QIcon());

    QUrl url() const;

    const QString &name() const;
    const QString &user() const;
    const QString &repo() const;
    QIcon icon() const;

private:
    QString name_;
    QString user_;
    QString repo_;
    QIcon icon_;
};

#endif // GITHUBREPOINFO_H
