#include "localmoditemwidget.h"
#include "ui_localmoditemwidget.h"

#include <QMenu>
#include <QMessageBox>
#include <QInputDialog>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QPainter>
#include <QStyle>

#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"
#include "ui/curseforge/curseforgemoddialog.h"
#include "ui/modrinth/modrinthmoddialog.h"
#include "ui/local/localmodmenu.h"
#include "util/tutil.hpp"
#include "util/funcutil.h"

namespace {
//「已禁用」的灰化交给 QSS（QLabel[modDisabled="true"]），颜色随主题走。
//原来在这里写死 color:#777，深色主题下对比度只有约 2.2:1，几乎读不出来。
//动态属性改了之后必须重新 polish，否则样式不会重新求值。
void setDisabledLook(const QList<QLabel *> &labels, bool disabled)
{
    for(auto *label : labels){
        if(label->property("modDisabled").toBool() == disabled)
            continue;
        label->setProperty("modDisabled", disabled);
        label->style()->unpolish(label);
        label->style()->polish(label);
    }
}
}

LocalModItemWidget::LocalModItemWidget(QWidget *parent, LocalMod *mod) :
    QWidget(parent),
    ui(new Ui::LocalModItemWidget),
    mod_(mod)
{
    //init ui
    ui->setupUi(this);
    ui->modName->setProperty("class", "Title");
    ui->modDescription->setProperty("class", "Description");
    ui->updateProgress->setVisible(false);
    ui->updateButton->setVisible(false);
    ui->curseforgeButton->setVisible(false);
    ui->modrinthButton->setVisible(false);
    ui->disableButton->setVisible(false);
    ui->featuredButton->setVisible(false);
    ui->tagsWidget->setMod(mod_);
    ui->tagsWidget->setContextMenuPolicy(Qt::DefaultContextMenu);

    this->setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_StyledBackground, true);
    setProperty("class", "LocalModItemWidget");

    // 三个菜单只建一次，内容在 updateInfo()/updateReady() 里清空重建
    rollbackMenu_ = new QMenu(this);
    ui->rollbackButton->setMenu(rollbackMenu_);
    updateMenu_ = new QMenu(this);
    ignoreUpdateMenu_ = new QMenu(tr("Ignore update"), this);
    ui->updateButton->setMenu(updateMenu_);

    onIconChanged();
    connect(mod_, &LocalMod::modIconUpdated, this, &LocalModItemWidget::onIconChanged);

    //init info
    updateInfo();

    //signals / slots
    connect(mod_, &LocalMod::modInfoChanged, this, &LocalModItemWidget::updateInfo);

    connect(mod_, &LocalMod::updateReady, this, &LocalModItemWidget::updateReady);

    connect(mod_, &LocalMod::curseforgeReady, this, &LocalModItemWidget::curseforgeReady);
    connect(mod_, &LocalMod::checkCurseforgeUpdateStarted, this, &LocalModItemWidget::startCheckCurseforgeUpdate);

    connect(mod_, &LocalMod::modrinthReady, this, &LocalModItemWidget::modrinthReady);
    connect(mod_, &LocalMod::checkModrinthUpdateStarted, this, &LocalModItemWidget::startCheckModrinthUpdate);

    connect(mod_, &LocalMod::updateStarted, this, &LocalModItemWidget::startUpdate);
    connect(mod_, &LocalMod::updateProgress, this, &LocalModItemWidget::updateProgress);
    connect(mod_, &LocalMod::updateFinished, this, &LocalModItemWidget::finishUpdate);
}

LocalModItemWidget::~LocalModItemWidget()
{
    delete ui;
}

#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
void LocalModItemWidget::enterEvent(QEnterEvent *event)
#else
void LocalModItemWidget::enterEvent(QEvent * event)
#endif
{
    ui->disableButton->setVisible(true);
    ui->featuredButton->setVisible(true);
    QWidget::enterEvent(event);
}

void LocalModItemWidget::leaveEvent(QEvent *event)
{
    ui->disableButton->setVisible(ui->disableButton->isChecked());
    ui->featuredButton->setVisible(ui->featuredButton->isChecked());
    QWidget::leaveEvent(event);
}

void LocalModItemWidget::paintEvent(QPaintEvent *event[[maybe_unused]])
{
    if(!mod_->modFile()) return;
    if(mod_->isDisabled()){
        QPainter p(this);
        p.setPen(Qt::NoPen);
        p.setOpacity(0.5);
        p.setBrush(Qt::gray);
        auto r = rect();
        p.drawRect(r);
    }
}

void LocalModItemWidget::updateInfo()
{
    auto setEffect = [=](QWidget *widget, const QString str){
        if(str.contains("</span>")){
            auto *effect = new QGraphicsDropShadowEffect;
            effect->setBlurRadius(4);
            effect->setColor(Qt::darkGray);
            effect->setOffset(1, 1);
            widget->setGraphicsEffect(effect);
        } else
            widget->setGraphicsEffect(nullptr);
    };

    //mod name
    auto displayName = mod_->displayName();
    //description
    auto description = mod_->commonInfo()->description();
    ui->modVersion->setText(mod_->commonInfo()->version());
    //authors
    if (!mod_->commonInfo()->authors().isEmpty()){
        auto authors = mod_->commonInfo()->authors().join("</b>, <b>").prepend("by <b>").append("</b>");
        setEffect(ui->modAuthors, authors);
        ui->modAuthors->setText(authors);
    }
    else
        ui->modAuthors->setText("");

    if (mod_->curseforgeMod())
        ui->curseforgeButton->setVisible(true);
    if (mod_->modrinthMod())
        ui->modrinthButton->setVisible(true);

    updateReady();

    //rollback
    rollbackMenu_->clear();
    if (mod_->oldFiles().isEmpty())
        ui->rollbackButton->setVisible(false);
    else{
        ui->rollbackButton->setVisible(true);
        ui->rollbackButton->setEnabled(true);
        for (const auto &file : mod_->oldFiles())
            connect(rollbackMenu_->addAction(file->commonInfo()->version()), &QAction::triggered, this, [=]{
                        ui->rollbackButton->setEnabled(false);
                        mod_->rollback(file);
                    });
    }

    //warning
    if (!mod_->duplicateFiles().isEmpty()){
        ui->warningButton->setToolTip(tr("Duplicate mod!"));
    } else{
        ui->warningButton->setVisible(false);
    }

    //enabled
    if (mod_->isDisabled()){
        ui->disableButton->setChecked(true);
        ui->disableButton->setVisible(true);
        displayName = clearFormat(displayName);
        description = clearFormat(description);
    } else{
        ui->disableButton->setChecked(false);
    }
    setDisabledLook({ui->modName, ui->modAuthors, ui->modDescription, ui->modVersion}, mod_->isDisabled());
    setEffect(ui->modName, displayName);
    ui->modName->setText(displayName);
    setEffect(ui->modDescription, description);
    ui->modDescription->setText(description);

    //featured
    if (mod_->isFeatured())
        ui->featuredButton->setVisible(true);
    ui->featuredButton->setChecked(mod_->isFeatured());

    updateUi();
}

void LocalModItemWidget::updateUi()
{
    Config config;
    ui->modAuthors->setVisible(config.getShowModAuthors());
    //tags
    ui->tagsWidget->updateUi();
}

void LocalModItemWidget::onIconChanged()
{
    ui->modIcon->setPixmap(mod_->icon().scaled(ui->modIcon->size(), Qt::KeepAspectRatio));
}

void LocalModItemWidget::on_updateButton_clicked()
{
    mod_->update();
}

void LocalModItemWidget::updateReady()
{
    // 两个菜单都要清空：QMenu::clear() 只删除"父对象是本菜单"的 action，
    // 子菜单里的 action 归子菜单所有，不清就会一直累积。
    updateMenu_->clear();
    ignoreUpdateMenu_->clear();

    if (mod_->updateTypes().isEmpty()){
        ui->updateButton->setVisible(false);
        return;
    }
    ui->updateButton->setVisible(true);
    ui->updateButton->setText(tr("Update"));
    ui->updateButton->setEnabled(true);

    for(auto &&fileInfo : mod_->curseforgeUpdater().updateFileInfos()){
        auto name = fileInfo.displayName();
        auto action = updateMenu_->addAction(QIcon(":/image/curseforge.svg"), name);
        connect(action, &QAction::triggered, this, [=]{
            mod_->update(fileInfo);
        });
        connect(ignoreUpdateMenu_->addAction(QIcon(":/image/curseforge.svg"), name), &QAction::triggered, this, [=]{
            mod_->ignoreUpdate(fileInfo);
        });
    }
    for(auto &&fileInfo : mod_->modrinthUpdater().updateFileInfos()){
        auto name = fileInfo.displayName();
        auto action = updateMenu_->addAction(QIcon(":/image/modrinth.svg"), name);
        connect(action, &QAction::triggered, this, [=]{
            mod_->update(fileInfo);
        });
        connect(ignoreUpdateMenu_->addAction(QIcon(":/image/modrinth.svg"), name), &QAction::triggered, this, [=]{
            mod_->ignoreUpdate(fileInfo);
        });
    }
    updateMenu_->addSeparator();
    if(!mod_->curseforgeUpdater().ignores().isEmpty() || !mod_->modrinthUpdater().ignores().isEmpty())
        updateMenu_->addAction(tr("Clear Update Ignores"), this, [=]{
            mod_->clearIgnores();
        });
    updateMenu_->addMenu(ignoreUpdateMenu_);
}

void LocalModItemWidget::startCheckCurseforge()
{
    //TODO
}

void LocalModItemWidget::curseforgeReady(bool bl)
{
    ui->curseforgeButton->setVisible(bl);
}

void LocalModItemWidget::startCheckCurseforgeUpdate()
{
    //TODO
}

void LocalModItemWidget::startCheckModrinth()
{
    //TODO
}

void LocalModItemWidget::modrinthReady(bool bl)
{
    ui->modrinthButton->setVisible(bl);
}

void LocalModItemWidget::startCheckModrinthUpdate()
{
    //TODO
}

void LocalModItemWidget::startUpdate()
{
    ui->updateButton->setText(tr("Updating"));
    ui->updateButton->setEnabled(false);
    ui->updateProgress->setVisible(true);
}

void LocalModItemWidget::updateProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->updateProgress->setMaximum(bytesTotal);
    ui->updateProgress->setValue(bytesReceived);
}

void LocalModItemWidget::finishUpdate(bool success)
{
    ui->updateProgress->setVisible(false);
    if (success)
        ui->updateButton->setVisible(false);
    else
    {
        ui->updateButton->setText(tr("Retry Update"));
        ui->updateButton->setEnabled(true);
    }
}

void LocalModItemWidget::on_curseforgeButton_clicked()
{
    auto curseforgeMod = mod_->curseforgeMod();
    if (!curseforgeMod->modInfo().hasBasicInfo())
        curseforgeMod->acquireBasicInfo();
    auto dialog = new CurseforgeModDialog(this, curseforgeMod, mod_);
    dialog->show();
}

void LocalModItemWidget::on_modrinthButton_clicked()
{
    auto modrinthMod = mod_->modrinthMod();
    if (!modrinthMod->modInfo().hasBasicInfo())
        modrinthMod->acquireFullInfo();
    auto dialog = new ModrinthModDialog(this, modrinthMod, mod_);
    dialog->show();
}

void LocalModItemWidget::on_warningButton_clicked()
{
    QString str = tr("Duplicate version of <b>%1</b> was found:").arg(mod_->commonInfo()->name());
    QStringList list(mod_->commonInfo()->version());
    for (const auto &file : mod_->duplicateFiles())
        list << file->commonInfo()->version();
    str += "<ul><li>" + list.join("</li><li>") + "</li></ul>";
    str += tr("Keep one of them and set the others as old mods?");
    if (QMessageBox::Yes == QMessageBox::question(this, tr("Incompatibility"), str))
        mod_->duplicateToOld();
}

void LocalModItemWidget::on_disableButton_toggled(bool checked)
{
    ui->disableButton->setEnabled(false);
    mod_->setEnabled(!checked);
    ui->disableButton->setEnabled(true);
}

void LocalModItemWidget::on_featuredButton_toggled(bool checked)
{
    ui->featuredButton->setEnabled(false);
    ui->featuredButton->setIcon(QIcon::fromTheme(checked ? "starred-symbolic" : "non-starred-symbolic"));
    mod_->setFeatured(checked);
    ui->featuredButton->setEnabled(true);
}

LocalMod *LocalModItemWidget::mod() const
{
    return mod_;
}

void LocalModItemWidget::setMinimal(bool minimal)
{
    if(minimal){
        tweakWidgetFontPointSize(ui->modDescription, 9);
        tweakWidgetFontPointSize(ui->modName, 11);
        ui->modIcon->setFixedSize(QSize(64, 64));
    } else{
        tweakWidgetFontPointSize(ui->modDescription, 10);
        tweakWidgetFontPointSize(ui->modName, 13);
        ui->modIcon->setFixedSize(QSize(80, 80));
    }
    onIconChanged();
//    ui->tagsWidget->setHidden(minimal);
//    ui->curseforgeButton->setHidden(minimal);
//    ui->modrinthButton->setHidden(minimal);
//    ui->featuredButton->setHidden(minimal);
//    ui->disableButton->setHidden(minimal);
}
