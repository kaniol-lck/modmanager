#ifndef REPLAYMODITEMWIDGET_H
#define REPLAYMODITEMWIDGET_H

#include <QWidget>

class ReplayMod;
class LocalModPath;

namespace Ui {
class ReplayModItemWidget;
}

class ReplayModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReplayModItemWidget(QWidget *parent, ReplayMod *mod);
    ~ReplayModItemWidget();

    ReplayMod *mod() const;

public slots:
    void setDownloadPath(LocalModPath *newDownloadPath);

private slots:
    void on_downloadButton_clicked();

private:
    Ui::ReplayModItemWidget *ui;
    ReplayMod *mod_;
    LocalModPath *downloadPath_ = nullptr;
};

#endif // REPLAYMODITEMWIDGET_H
