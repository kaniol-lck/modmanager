#include "localmoditem.h"

#include "ui/local/localmodbrowser.h"
#include "local/localmod.h"
#include "curseforge/curseforgemod.h"
#include "modrinth/modrinthmod.h"
#include "util/funcutil.h"

LocalModItem::LocalModItem(LocalMod *mod) :
    mod_(mod)
{}

bool LocalModItem::operator<(const QStandardItem &other) const
{
    auto otherItem = dynamic_cast<const LocalModItem &>(other);
    auto &&otherMod = otherItem.mod_;
    Config config;
    if(config.getStarredAtTop()){
        if(mod_->isFeatured() && !otherMod->isFeatured()) return true;
        if(!mod_->isFeatured() && otherMod->isFeatured()) return false;
    }
    if(config.getDisabedAtBottom()){
        if(mod_->isDisabled() && !otherMod->isDisabled()) return false;
        if(!mod_->isDisabled() && otherMod->isDisabled()) return true;
    }
    if(column() == LocalModBrowser::FileDateColumn)
        return data(FileDateRole).toDateTime() < other.data(FileDateRole).toDateTime();
    if(column() == LocalModBrowser::FileSizeColumn)
        return data(FileSizeRole).toInt() < other.data(FileSizeRole).toInt();
    if(column() == LocalModBrowser::TagsColumn)
        return data(TagsRole).toString() < other.data(TagsRole).toString();;
    return QStandardItem::operator<(other);
}

QList<QStandardItem *> LocalModItem::itemsFromMod(LocalMod *mod)
{
    auto item = new LocalModItem(mod);
    item->setData(QVariant::fromValue(mod));
    auto nameItem = new LocalModItem(mod);
    auto idItem = new LocalModItem(mod);
    auto versionItem = new LocalModItem(mod);
    auto enableItem= new LocalModItem(mod);
    auto starItem = new LocalModItem(mod);
    auto tagsItem = new LocalModItem(mod);
    auto fileDateItem = new LocalModItem(mod);
    auto fileSizeItem = new LocalModItem(mod);
    auto fileNameItem = new LocalModItem(mod);
    auto descItem = new LocalModItem(mod);
    auto curseforgeIdItem = new LocalModItem(mod);
    auto curseforgeFileIdItem = new LocalModItem(mod);
    auto modrinthFileIdItem = new LocalModItem(mod);
    auto modrinthIdItem = new LocalModItem(mod);
    QList<QStandardItem *> list = { item, nameItem, idItem, versionItem, enableItem, starItem, tagsItem, fileDateItem, fileSizeItem, fileNameItem, curseforgeIdItem, curseforgeFileIdItem, modrinthIdItem, modrinthFileIdItem, descItem };
    auto onModChanged = [=]{
        nameItem->setText(clearFormat(mod->displayName()));
        idItem->setText(mod->commonInfo()->id());
        versionItem->setText(mod->commonInfo()->version());
        descItem->setText(mod->commonInfo()->description());
        for(auto &&item : list)
            item->setForeground(mod->isDisabled()? Qt::gray : Qt::black);
        if(mod->curseforgeMod())
            curseforgeIdItem->setText(QString::number(mod->curseforgeMod()->modInfo().id()));
        if(auto file = mod->curseforgeUpdate().currentFileInfo())
            curseforgeFileIdItem->setText(QString::number(file->id()));
        if(mod->modrinthMod())
            modrinthIdItem->setText(mod->modrinthMod()->modInfo().id());
        if(auto file = mod->modrinthUpdate().currentFileInfo())
            modrinthFileIdItem->setText(file->id());
        fileDateItem->setText(mod->modFile()->fileInfo().lastModified().toString());
        fileDateItem->setData(mod->modFile()->fileInfo().lastModified(), FileDateRole);
        fileSizeItem->setText(sizeConvert(mod->modFile()->fileInfo().size()));
        fileSizeItem->setData(mod->modFile()->fileInfo().size(), FileSizeRole);
        fileNameItem->setText(mod->modFile()->fileInfo().fileName());
        QString tagsStr;
        for(auto &&tag : mod->tagManager().tags())
            tagsStr += tag.name() + " ";
        tagsItem->setData(tagsStr, TagsRole);
    };
    auto onIconChanged = [=]{
        nameItem->setIcon(mod->icon().scaled(96, 96));
    };
    onModChanged();
    onIconChanged();
    QObject::connect(mod, &LocalMod::modFileUpdated, onModChanged);
    QObject::connect(mod, &LocalMod::modIconUpdated, onIconChanged);
    return list;
}
