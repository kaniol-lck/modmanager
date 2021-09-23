#ifndef OPTIFINEMODITEMWIDGET_H
#define OPTIFINEMODITEMWIDGET_H

#include <QWidget>

class OptifineMod;
class LocalModPath;

namespace Ui {
class OptifineModItemWidget;
}

class OptifineModItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OptifineModItemWidget(QWidget *parent, OptifineMod *mod);
    ~OptifineModItemWidget();

    OptifineMod *mod() const;

public slots:
    void setDownloadPath(LocalModPath *newDownloadPath);

private slots:
    void on_downloadButton_clicked();

private:
    Ui::OptifineModItemWidget *ui;
    OptifineMod *mod_;
    LocalModPath *downloadPath_ = nullptr;
};

#endif // OPTIFINEMODITEMWIDGET_H
