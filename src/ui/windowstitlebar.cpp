#include "windowstitlebar.h"
#include "ui_windowstitlebar.h"

#include <QMenuBar>
#include <QStyle>
#include <QDebug>
#include <QMouseEvent>
#include <QToolBar>
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
        menuBar_->hide();
        for(auto &&widget : menuButtons_)
            ui->menuLayout->removeWidget(widget);
        qDeleteAll(menuButtons_);
        menuButtons_.clear();
        for(auto &&action : menuBar_->actions()){
            auto button = new QToolButton(this);
            button->setAutoRaise(true);
            button->setProperty("class", "MenuButton");
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            button->setDefaultAction(action);
            button->setPopupMode(QToolButton::InstantPopup);
            button->setStyleSheet("QToolButton::menu-indicator{image:none;}");
            ui->menuLayout->addWidget(button);
            menuButtons_ << button;
        }
        //    menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        //    menuBar->adjustSize();
    }
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


void WindowsTitleBar::on_WindowsTitleBar_customContextMenuRequested(const QPoint &pos)
{
    //    parent_->menu
}

void WindowsTitleBar::setParentWidget(QWidget *newParentWidget)
{
    parentWidget_ = newParentWidget;
}

