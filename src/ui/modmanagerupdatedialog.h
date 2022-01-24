#ifndef MODMANAGERUPDATEDIALOG_H
#define MODMANAGERUPDATEDIALOG_H

#include <QDialog>

#include "github/githubreleaseinfo.h"
#include "network/reply.hpp"

namespace Ui {
class ModManagerUpdateDialog;
}

class ModManagerUpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModManagerUpdateDialog(QWidget *parent = nullptr);
    ~ModManagerUpdateDialog();

    void checkVersion();

    const GitHubReleaseInfo &updateRelease() const;
    const QString &updateVersion() const;

signals:
    void updateChecked(bool hasUpdate);
    void updateConfirmed(const GitHubFileInfo &fileInfo);

private slots:
    void on_ModManagerUpdateDialog_accepted();

    void on_pushButton_clicked();

private:
    Ui::ModManagerUpdateDialog *ui;
    std::unique_ptr<Reply<QList<GitHubReleaseInfo> > > checkUpdateReply_;
    GitHubReleaseInfo updateRelease_;
    GitHubFileInfo updateFile_;
};

#endif // MODMANAGERUPDATEDIALOG_H
