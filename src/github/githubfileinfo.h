#ifndef GITHUBFILEINFO_H
#define GITHUBFILEINFO_H

#include <QUrl>
#include <QVariant>

class GitHubFileInfo
{
public:
    GitHubFileInfo() = default;

    static GitHubFileInfo fromVariant(const QVariant &variant);

    const QString &name() const;
    qint64 size() const;
    const QUrl &url() const;
    const QUrl &downloadUrl() const;

private:
    QString name_;
    qint64 size_ = 0;
    QUrl url_;
    QUrl downloadUrl_;
};

#endif // GITHUBFILEINFO_H
