#include "localmoditemwidget.h"
#include "ui_localmoditemwidget.h"

#include <QMenu>
#include <QMessageBox>
#include <QInputDialog>

#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"
#include "ui/curseforge/curseforgemoddialog.h"
#include "ui/modrinth/modrinthmoddialog.h"
#include "util/tutil.hpp"

LocalModItemWidget::LocalModItemWidget(QWidget *parent, LocalMod *mod) : QWidget(parent),
                                                                         ui(new Ui::LocalModItemWidget),
                                                                         mod_(mod)
{
    //init ui
    ui->setupUi(this);
    ui->updateProgress->setVisible(false);
    ui->updateButton->setVisible(false);
    ui->updateButton->setEnabled(false);
    ui->curseforgeButton->setEnabled(false);
    ui->modrinthButton->setEnabled(false);
    ui->disableButton->setVisible(false);
    ui->featuredButton->setVisible(false);

    this->setAttribute(Qt::WA_Hover, true);

    //init info
    updateInfo();

    //signals / slots
    connect(mod_, &LocalMod::modFileUpdated, this, &LocalModItemWidget::updateInfo);

    connect(mod_, &LocalMod::updateReady, this, &LocalModItemWidget::updateReady);

    connect(mod_, &LocalMod::checkCurseforgeStarted, this, &LocalModItemWidget::startCheckCurseforge);
    connect(mod_, &LocalMod::curseforgeReady, this, &LocalModItemWidget::curseforgeReady);
    connect(mod_, &LocalMod::checkCurseforgeUpdateStarted, this, &LocalModItemWidget::startCheckCurseforgeUpdate);

    connect(mod_, &LocalMod::checkModrinthStarted, this, &LocalModItemWidget::startCheckModrinth);
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

void LocalModItemWidget::enterEvent(QEvent *event)
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

void LocalModItemWidget::updateInfo()
{
    ui->modName->setText(mod_->displayName());
    ui->modVersion->setText(mod_->commonInfo()->version());
    auto description = mod_->commonInfo()->description();
    auto index = description.indexOf(".");
    if (index > 0)
        description = description.left(index + 1);
    ui->modDescription->setText(description);
    if (!mod_->commonInfo()->authors().isEmpty())
        ui->modAuthors->setText(mod_->commonInfo()->authors().join("</b>, <b>").prepend("by <b>").append("</b>"));
    else
        ui->modAuthors->setText("");

    if (!mod_->commonInfo()->iconBytes().isEmpty())
    {
        QImage image;
        image.loadFromData(mod_->commonInfo()->iconBytes());
        if (mod_->modFile()->type() == LocalModFile::Disabled)
        {
            auto alphaChannel = image.convertToFormat(QImage::Format_Alpha8);
            image = image.convertToFormat(QImage::Format_Grayscale8);
            image.setAlphaChannel(alphaChannel);
        }
        QPixmap pixelmap;
        pixelmap.convertFromImage(image);
        ui->modIcon->setPixmap(pixelmap.scaled(80, 80, Qt::KeepAspectRatio));
    }
    else
    {
        QPixmap pixelmap(":/image/modmanager.png");
        ui->modIcon->setPixmap(pixelmap.scaled(80, 80, Qt::KeepAspectRatio));
    }

    if (mod_->curseforgeMod())
        ui->curseforgeButton->setEnabled(true);
    if (mod_->modrinthMod())
        ui->modrinthButton->setEnabled(true);

    updateReady(mod_->defaultUpdateType());

    //rollback
    if (mod_->oldFiles().isEmpty())
        ui->rollbackButton->setVisible(false);
    else
    {
        ui->rollbackButton->setVisible(true);
        ui->rollbackButton->setEnabled(true);
        auto menu = new QMenu(this);
        for (const auto &file : mod_->oldFiles())
            connect(menu->addAction(file->commonInfo()->version()), &QAction::triggered, this, [=]
                    {
                        ui->rollbackButton->setEnabled(false);
                        mod_->rollback(file);
                    });
        ui->rollbackButton->setMenu(menu);
    }

    //warning
    if (!mod_->duplicateFiles().isEmpty())
    {
        ui->warningButton->setToolTip(tr("Duplicate mod!"));
    }
    else
    {
        ui->warningButton->setVisible(false);
    }

    //enabled
    if (mod_->isDisabled())
    {
        ui->disableButton->setChecked(true);
        ui->disableButton->setVisible(true);
        ui->modName->setStyleSheet("color: #777");
        ui->modAuthors->setStyleSheet("color: #777");
        ui->modDescription->setStyleSheet("color: #777;");
        ui->modVersion->setStyleSheet("color: #777");
        ui->updateButton->setEnabled(false);
    }
    else
    {
        ui->disableButton->setChecked(false);
        ui->modName->setStyleSheet("");
        ui->modAuthors->setStyleSheet("");
        ui->modDescription->setStyleSheet("");
        ui->modVersion->setStyleSheet("");
        ui->updateButton->setEnabled(true);
    }

    //featured
    if (mod_->isFeatured())
        ui->featuredButton->setVisible(true);
    ui->featuredButton->setChecked(mod_->isFeatured());

    //tags
    QStringList tagTextList;
    for(auto widget : qAsConst(tagWidgets_)){
        ui->tagsLayout->removeWidget(widget);
        widget->deleteLater();
    }
    tagWidgets_.clear();
    for(auto &&tag : mod_->tags()){
        auto label = new QLabel(tag.name(), this);
        label->setToolTip(tag.name());
        label->setStyleSheet(QString("color: #fff; background-color: %1; border-radius:10px; padding:2px 4px;").arg(tag.tagCategory().color().name()));
        ui->tagsLayout->addWidget(label);
        tagWidgets_ << label;
    }
}

void LocalModItemWidget::on_updateButton_clicked()
{
    mod_->update(mod_->defaultUpdateType());
}

void LocalModItemWidget::updateReady(LocalMod::ModWebsiteType type)
{
    if (type == LocalMod::None)
        return;
    ui->updateButton->setVisible(true);
    ui->updateButton->setEnabled(true);
    if (type == LocalMod::Curseforge)
        ui->updateButton->setIcon(QIcon(":/image/curseforge.svg"));
    else if (type == LocalMod::Modrinth)
        ui->updateButton->setIcon(QIcon(":/image/modrinth.svg"));

    if (mod_->updateTypes().size() == 1)
        return;
    auto menu = new QMenu(this);
    for (auto type2 : mod_->updateTypes())
    {
        if (type == type2)
            continue;
        if (type2 == LocalMod::Curseforge)
            connect(menu->addAction(QIcon(":/image/curseforge.svg"), "Curseforge"), &QAction::triggered, this, [=]
                    {
                        ui->updateButton->setIcon(QIcon(":/image/curseforge.svg"));
                        mod_->update(LocalMod::Curseforge);
                    });
        else if (type2 == LocalMod::Modrinth)
            connect(menu->addAction(QIcon(":/image/modrinth.svg"), "Modrinth"), &QAction::triggered, this, [=]
                    {
                        ui->updateButton->setIcon(QIcon(":/image/modrinth.svg"));
                        mod_->update(LocalMod::Modrinth);
                    });
    }
    ui->updateButton->setMenu(menu);
}

void LocalModItemWidget::startCheckCurseforge()
{
    //TODO
}

void LocalModItemWidget::curseforgeReady(bool bl)
{
    ui->curseforgeButton->setEnabled(bl);
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
    ui->modrinthButton->setEnabled(bl);
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

void LocalModItemWidget::on_LocalModItemWidget_customContextMenuRequested(const QPoint &pos)
{
    auto menu = new QMenu(this);
    connect(menu->addAction(tr("Set Alias")), &QAction::triggered, this, [=]{
        bool ok;
        auto alias = QInputDialog::getText(this, tr("Set mod alias"), tr("Alias of <b>%1</b> mod:").arg(mod_->commonInfo()->name()), QLineEdit::Normal, mod_->alias(), &ok);
        if(ok)
            mod_->setAlias(alias);
    });
    auto addTagmenu = menu->addMenu(tr("Add tag"));
    //Type category
    auto addTypeTagmenu = addTagmenu->addMenu(tr("Type tag"));
    for(const auto &tag : Tag::typeTags())
        connect(addTypeTagmenu->addAction(tag.name()), &QAction::triggered, this, [=]{
            mod_->addTag(tag);
        });
    //Functionality category
    auto addFunctionalityTagmenu = addTagmenu->addMenu(tr("Functionality tag"));
    for(auto &&tag : Tag::functionalityTags())
        connect(addFunctionalityTagmenu->addAction(tag.name()), &QAction::triggered, this, [=]{
            mod_->addTag(tag);
        });
    if(!addFunctionalityTagmenu->isEmpty())
        addFunctionalityTagmenu->addSeparator();
    connect(addFunctionalityTagmenu->addAction(tr("New functionality tag...")), &QAction::triggered, this, [=]{
        bool ok;
        auto name = QInputDialog::getText(this, tr("New tag"), tr("Functionality:"), QLineEdit::Normal, "", &ok);
        if(ok && !name.isEmpty())
            mod_->addTag(Tag(name, TagCategory::FunctionalityCategory));
    });
    //TODO: other categories
    //Translation category
    connect(addTagmenu->addAction(tr("Translation tag")), &QAction::triggered, this, [=]{
        bool ok;
        QString str;
        if(auto translationTag = mod_->tagManager().translationTag())
            str = translationTag->name();
        auto name = QInputDialog::getText(this, tr("Translation tag"), tr("Translation:"), QLineEdit::Normal, str, &ok);
        if(ok && !name.isEmpty())
            mod_->addTag(Tag(name, TagCategory::TranslationCategory));
    });
    //Notation category
    connect(addTagmenu->addAction(tr("Notation tag")), &QAction::triggered, this, [=]{
        bool ok;
        QString str;
        if(auto notationTag = mod_->tagManager().notationTag())
            str = notationTag->name();
        auto name = QInputDialog::getText(this, tr("Notation tag"), tr("Notation:"), QLineEdit::Normal, str, &ok);
        if(ok && !name.isEmpty())
            mod_->addTag(Tag(name, TagCategory::NotationCategory));
    });
    //Custom category
    for(auto &&tag : Tag::customTags())
        connect(addTagmenu->addAction(tag.name()), &QAction::triggered, this, [=]{
            mod_->addTag(tag);
        });
    addTagmenu->addSeparator();
    connect(addTagmenu->addAction(tr("New custom tag...")), &QAction::triggered, this, [=]{
        bool ok;
        auto name = QInputDialog::getText(this, tr("New tag"), tr("New tag name:"), QLineEdit::Normal, "", &ok);
        if(ok)
            mod_->addTag(Tag(name));
    });
    //TODO: tag manage
    if(!mod_->tags().isEmpty()){
        auto removeTagmenu = menu->addMenu(tr("remove tag"));
        for(const auto &tag : mod_->tags())
            connect(removeTagmenu->addAction(tag.name()), &QAction::triggered, this, [=]{
                mod_->removeTag(tag);
            });
    }
    menu->addSeparator();
    auto starAction = menu->addAction(tr("Star"));
    starAction->setCheckable(true);
    starAction->setChecked(mod_->isFeatured());
    connect(starAction, &QAction::toggled, this, [=](bool bl){
        mod_->setFeatured(bl);
    });
    auto disableAction = menu->addAction(tr("Disable"));
    disableAction->setCheckable(true);
    disableAction->setChecked(mod_->isDisabled());
    connect(disableAction, &QAction::toggled, this, [=](bool bl){
        mod_->setEnabled(!bl);
    });
    if(!menu->actions().isEmpty())
        menu->exec(mapToGlobal(pos));
}

