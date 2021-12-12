#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = nullptr);
    ~Preferences();
private slots:
    void on_Preferences_accepted();
    void on_commonPathButton_clicked();
    void on_downloadPathButton_clicked();
    void on_useFramelessWindow_toggled(bool checked);

    void on_proxyType_currentIndexChanged(int index);

private:
    Ui::Preferences *ui;
};

#endif // PREFERENCES_H
