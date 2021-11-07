#ifndef CURSEFORGEIMAGEPOPUP_H
#define CURSEFORGEIMAGEPOPUP_H

#include <QDialog>

namespace Ui {
class CurseforgeImagePopup;
}

class CurseforgeImagePopup : public QDialog
{
    Q_OBJECT

public:
    explicit CurseforgeImagePopup(QWidget *parent, const QUrl &url, const QString &title, const QString &desciption, const QByteArray &bytes);
    ~CurseforgeImagePopup();
protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
private:
    Ui::CurseforgeImagePopup *ui;
    QPoint clickPos_;
};

#endif // CURSEFORGEIMAGEPOPUP_H
