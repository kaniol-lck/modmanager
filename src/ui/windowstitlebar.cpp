#include "windowstitlebar.h"
#include "ui_windowstitlebar.h"

#include <QMenuBar>
#include <QStyle>
#include <QDebug>
#include <QMouseEvent>
#include <QToolBar>
#include <QPainter>
#include <QStyleOptionMenuItem>
#ifdef Q_OS_WIN
#include <windowsx.h>
#include <dwmapi.h>
#endif

WindowsTitleBar::WindowsTitleBar(QWidget *parent, const QString &title, QMenuBar *menuBar) :
    QWidget(parent),
    ui(new Ui::WindowsTitleBar),
    parentWidget_(parent),
    menuBar_(menuBar)
{
    ui->setupUi(this);
//    setMouseTracking(true);
    ui->closeButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    ui->maxButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    ui->minButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarMinButton));
    ui->titleText->setText(title);
    updateMenuBar();
}

WindowsTitleBar::~WindowsTitleBar()
{
    delete ui;
}

void WindowsTitleBar::setIconVisible(bool bl)
{
    ui->icon->setVisible(bl);
}

void WindowsTitleBar::updateMenuBar()
{
    if(menuBar_){
        ui->verticalLayout->addWidget(menuBar_);
        // I don't know why 5px + 31px, but it works!
        menuBar_->setStyleSheet("QMenuBar {background-color: transparent; margin-bottom: 5px; min-height:31px;}");
    }
}

void WindowsTitleBar::setMaximumed()
{
    ui->maxButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarNormalButton));
}

void WindowsTitleBar::setNormal()
{
    ui->maxButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarMaxButton));
}

#ifdef Q_OS_WIN
void WindowsTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons()&Qt::LeftButton)
        parentWidget_->move(event->pos() + parentWidget_->pos() - clickPos_);
}

void WindowsTitleBar::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
        clickPos_=event->pos();
}
#endif //Q_OS_WIN

void WindowsTitleBar::on_closeButton_clicked()
{
    parentWidget_->close();
}

void WindowsTitleBar::on_maxButton_clicked()
{
    if (parentWidget_->isMaximized()){
        parentWidget_->showNormal();
        ui->maxButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    }
    else{
        parentWidget_->showMaximized();
        ui->maxButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarNormalButton));
    }
}

void WindowsTitleBar::on_minButton_clicked()
{
    parentWidget_->showMinimized();
}

void WindowsTitleBar::setParentWidget(QWidget *newParentWidget)
{
    parentWidget_ = newParentWidget;
}

#ifdef Q_OS_WIN
bool WindowsTitleBar::hitTest(QPoint pos, long *result)
{
    QWidget* child = childAt(pos);
    if(auto label = qobject_cast<QLabel*>(child); label == ui->icon){
        *result = HTSYSMENU;
        return true;
    }
    if(!qobject_cast<QToolButton*>(child) && !qobject_cast<QMenuBar*>(child)){
        *result = HTCAPTION;
        return true;
    }
    return false;
}
#endif //Q_OS_WIN
