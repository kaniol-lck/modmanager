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

#include "config.hpp"

WindowsTitleBar::WindowsTitleBar(QWidget *parent, QMenuBar *menuBar) :
    QWidget(parent),
    ui(new Ui::WindowsTitleBar),
    parentWidget_(parent),
    menuBar_(menuBar)
{
    ui->setupUi(this);
    setMouseTracking(true);
    ui->closeButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    ui->maxButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    ui->minButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarMinButton));
    updateMenuBar();
    auto updateWindowTitle = [=]{
        ui->titleText->setText(parent->windowTitle() + " - " +
                               qApp->applicationDisplayName());
    };
    updateWindowTitle();
    connect(parent, &QWidget::windowTitleChanged, this, updateWindowTitle);
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

void WindowsTitleBar::paintEvent(QPaintEvent *event)
{
    if(!Config().getEnableBlurBehind()) return;
    QPainter p(this);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.fillRect(rect(), QBrush(QColor(255, 255, 255, 195)));
}

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
#if QT_VERSION_MAJOR == 6
bool WindowsTitleBar::hitTest(QPoint pos, qintptr *result)
#else
bool WindowsTitleBar::hitTest(QPoint pos, long *result)
#endif  // QT_VERSION_MAJOR
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
