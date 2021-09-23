#ifndef OPTIFINEMODITEMWIDGET_H
#define OPTIFINEMODITEMWIDGET_H

#include <QWidget>

class OptifineMod;

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

private slots:
    void on_downloadButton_clicked();

private:
    Ui::OptifineModItemWidget *ui;
    OptifineMod *mod_;
};

#endif // OPTIFINEMODITEMWIDGET_H
