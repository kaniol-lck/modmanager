#ifndef DOWNLOADSTATUSBARWIDGET_H
#define DOWNLOADSTATUSBARWIDGET_H

#include <QWidget>

namespace Ui {
class DownloadStatusBarWidget;
}

class DownloadStatusBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadStatusBarWidget(QWidget *parent = nullptr);
    ~DownloadStatusBarWidget();

public slots:
    void setDownloadSpeed(qint64 download, qint64 upload);

private:
    Ui::DownloadStatusBarWidget *ui;
};

#endif // DOWNLOADSTATUSBARWIDGET_H
