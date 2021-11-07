#include "windowstitlebar.h"
#include "ui_windowstitlebar.h"

#include <QMenuBar>
#include <QStyle>
#include <QDebug>
#include <QMouseEvent>
#ifdef Q_OS_WIN
#include <windowsx.h>
#include <dwmapi.h>
#endif

WindowsTitleBar::WindowsTitleBar(QWidget *parent, const QString &title, QMenuBar *menuBar) :
    QWidget(parent),
    ui(new Ui::WindowsTitleBar),
    parent_(parent)
{
    ui->setupUi(this);
    ui->closeButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    ui->maxButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    ui->minButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarMinButton));
    ui->titleText->setText(title);
    if(menuBar){
//    menuBar->setStyleSheet("QMenuBar{background-color: transparent;}");
//    ui->verticalLayout->addWidget(menuBar);
        menuBar->hide();
        for(auto &&action : menuBar->actions()){
            auto button = new QToolButton(this);
            button->setAutoRaise(true);
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            button->setDefaultAction(action);
            button->setPopupMode(QToolButton::InstantPopup);
            button->setStyleSheet("QToolButton::menu-indicator{image:none;}");
            ui->menuLayout->addWidget(button);
        }
        //    menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        //    menuBar->adjustSize();
    }
}

WindowsTitleBar::~WindowsTitleBar()
{
    delete ui;
}

void WindowsTitleBar::setIconVisible(bool bl)
{
    ui->icon->setVisible(bl);
}

#ifdef Q_OS_WIN
void WindowsTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons()&Qt::LeftButton)
        parent_->move(event->pos() + parent_->pos() - clickPos_);
}

void WindowsTitleBar::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
        clickPos_=event->pos();
}

void WindowsTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    clickPos_ = QPoint();
}
#endif //Q_OS_WIN

void WindowsTitleBar::on_closeButton_clicked()
{
    parent_->close();
}

void WindowsTitleBar::on_maxButton_clicked()
{
    if (parent_->isMaximized()){
        parent_->showNormal();
        ui->maxButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    }
    else{
        parent_->showMaximized();
        ui->maxButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarNormalButton));
    }
}


void WindowsTitleBar::on_minButton_clicked()
{
    parent_->showMinimized();
}


void WindowsTitleBar::on_WindowsTitleBar_customContextMenuRequested(const QPoint &pos)
{
//    parent_->menu
}

