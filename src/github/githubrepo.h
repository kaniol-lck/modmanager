#ifndef GITHUBREPO_H
#define GITHUBREPO_H

#include <QObject>

class GitHubRepo : public QObject
{
    Q_OBJECT
public:
    explicit GitHubRepo(QObject *parent = nullptr);

signals:

};

#endif // GITHUBREPO_H
