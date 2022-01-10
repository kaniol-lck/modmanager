#ifndef REPLAYMODITEMWIDGET_H
#define REPLAYMODITEMWIDGET_H

#include <QWidget>

class ReplayMod;
class ReplayModBrowser;
namespace Ui {
class ReplayModItemWidget;
}

class ReplayModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReplayModItemWidget(ReplayModBrowser *parent, ReplayMod *mod);
    ~ReplayModItemWidget();

    ReplayMod *mod() const;

public slots:
    void onDownloadPathChanged();

private slots:
    void on_downloadButton_clicked();

private:
    Ui::ReplayModItemWidget *ui;
    ReplayModBrowser *browser_ = nullptr;
    ReplayMod *mod_;
};

#endif // REPLAYMODITEMWIDGET_H
