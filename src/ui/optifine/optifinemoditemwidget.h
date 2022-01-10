#ifndef OPTIFINEMODITEMWIDGET_H
#define OPTIFINEMODITEMWIDGET_H

#include <QWidget>

class OptifineMod;
class OptifineModBrowser;
namespace Ui {
class OptifineModItemWidget;
}

class OptifineModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OptifineModItemWidget(OptifineModBrowser *parent, OptifineMod *mod);
    ~OptifineModItemWidget();

    OptifineMod *mod() const;

public slots:
    void onDownloadPathChanged();

private slots:
    void on_downloadButton_clicked();

private:
    Ui::OptifineModItemWidget *ui;
    OptifineModBrowser *browser_ = nullptr;
    OptifineMod *mod_;
};

#endif // OPTIFINEMODITEMWIDGET_H
