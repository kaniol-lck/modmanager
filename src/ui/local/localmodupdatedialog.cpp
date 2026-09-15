#include "localmodupdatedialog.h"
#include "ui_localmodupdatedialog.h"

#include "local/localmodpath.h"
#include "local/localmod.h"
#include "local/localfilelinker.h"
#include "qss/stylesheets.h"

#include <QComboBox>

auto getToolTip = [](const auto &fileInfo){
    QStringList list;
    list << QObject::tr("File Name: %1").arg(fileInfo.fileName());
    list << QObject::tr("Date Time: %1").arg(fileInfo.fileDate().toString());
    QStringList gameVersions;
    for(const auto &version : fileInfo.gameVersions())
        gameVersions << version;
    list << QObject::tr("Game Version: %1").arg(gameVersions.join(" "));
    QStringList loaderTypes;
    for(const auto &loaderType : fileInfo.loaderTypes())
        loaderTypes << ModLoaderType::toString(loaderType);
    list << QObject::tr("Loader Type: %1").arg(loaderTypes.join(" "));
    return list.join('\n');
};

LocalModUpdateDialog::LocalModUpdateDialog(QWidget *parent, LocalModPath *modPath) :
    QDialog(parent),
    ui(new Ui::LocalModUpdateDialog),
    modPath_(modPath)
{
    ui->setupUi(this);
    ui->updateTreeView->setModel(&model_);
    ui->updateTreeView->header()->setSectionResizeMode(QHeaderView::Stretch);

    model_.setHorizontalHeaderItem(NameColumn, new QStandardItem(tr("Mod Name")));
    model_.setHorizontalHeaderItem(BeforeColumn, new QStandardItem(tr("Old Version")));
    model_.setHorizontalHeaderItem(AfterColumn, new QStandardItem(tr("New Version")));
    model_.setHorizontalHeaderItem(SourceColumn, new QStandardItem(tr("Update Source")));

    ui->updateTreeView->header()->setSectionResizeMode(NameColumn, QHeaderView::Fixed);
    ui->updateTreeView->setColumnWidth(NameColumn, 250);
    ui->updateTreeView->header()->setSectionResizeMode(SourceColumn, QHeaderView::Fixed);
    ui->updateTreeView->setColumnWidth(SourceColumn, 140);

    for(auto &&mod : modPath_->modList()){
        auto enabled = !mod->isDisabled();

        auto type = mod->defaultUpdateType();
        auto types = mod->updateTypes();
        if(type == ModWebsiteType::None) continue;

        auto nameItem = new QStandardItem();
        nameItem->setData(QVariant::fromValue(mod));
        auto beforeItem = new QStandardItem();
        auto afterItem = new QStandardItem();
        auto sourceItem = new QStandardItem();
        model_.appendRow({nameItem, beforeItem, afterItem, sourceItem});

        nameItem->setText(mod->commonInfo()->name());
        nameItem->setCheckable(true);
        nameItem->setCheckState(enabled? Qt::Checked : Qt::Unchecked);
        nameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        if(type == ModWebsiteType::Curseforge)
            beforeItem->setText(mod->modFile()->linker()->curseforgeFileInfo()->displayName());
        else if(type == ModWebsiteType::Modrinth)
            beforeItem->setText(mod->modFile()->linker()->modrinthFileInfo()->displayName());
        beforeItem->setForeground(Qt::darkRed);
        beforeItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        if(type == ModWebsiteType::Curseforge)
            beforeItem->setToolTip(getToolTip(*mod->modFile()->linker()->curseforgeFileInfo()));
        else if(type == ModWebsiteType::Modrinth)
            beforeItem->setToolTip(getToolTip(*mod->modFile()->linker()->modrinthFileInfo()));

        afterItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        afterItem->setForeground(themeColor(ThemeColor::UpdateAvailable));
        auto setAfterItem = [=](const auto &fileInfo){
            afterItem->setText(fileInfo.displayName());
            afterItem->setToolTip(getToolTip(fileInfo));
        };
        auto unsetAfterItem = [=]{
            afterItem->setText("");
            afterItem->setToolTip("");
        };

        auto setType = [=](auto type){
            afterItem->setData(0);
            switch (type) {
            case ModWebsiteType::None:
                break;
            case ModWebsiteType::Curseforge:
                if(mod->curseforgeUpdater().updateFileInfos().size() == 1){
                    setAfterItem(mod->curseforgeUpdater().updateFileInfos().first());
                    ui->updateTreeView->setIndexWidget(model_.indexFromItem(afterItem), nullptr);
                }else if(mod->curseforgeUpdater().updateFileInfos().size() > 1){
                    unsetAfterItem();
                    auto comboBox = new QComboBox(this);
                    comboBox->setStyleSheet(QString("color: %1").arg(themeColor(ThemeColor::UpdateAvailable).name()));
                    ui->updateTreeView->setIndexWidget(model_.indexFromItem(afterItem), comboBox);
                    for(const auto &fileInfo : mod->curseforgeUpdater().updateFileInfos())
                        comboBox->addItem(fileInfo.displayName());
                    afterItem->setToolTip(getToolTip(mod->curseforgeUpdater().updateFileInfos().first()));
                    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){
                        auto &fileInfo = mod->curseforgeUpdater().updateFileInfos().at(index);
                        afterItem->setData(index);
                        afterItem->setToolTip(getToolTip(fileInfo));
                    });
                }
                break;
            case ModWebsiteType::Modrinth:
                if(mod->modrinthUpdater().updateFileInfos().size() == 1){
                    setAfterItem(mod->modrinthUpdater().updateFileInfos().first());
                    ui->updateTreeView->setIndexWidget(model_.indexFromItem(afterItem), nullptr);
                }else if(mod->modrinthUpdater().updateFileInfos().size() > 1){
                    unsetAfterItem();
                    auto comboBox = new QComboBox(this);
                    comboBox->setStyleSheet(QString("color: %1").arg(themeColor(ThemeColor::UpdateAvailable).name()));
                    ui->updateTreeView->setIndexWidget(model_.indexFromItem(afterItem), comboBox);
                    for(const auto &fileInfo : mod->modrinthUpdater().updateFileInfos())
                        comboBox->addItem(fileInfo.displayName());
                    afterItem->setToolTip(getToolTip(mod->modrinthUpdater().updateFileInfos().first()));
                    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){
                        auto &fileInfo = mod->modrinthUpdater().updateFileInfos().at(index);
                        afterItem->setData(index);
                        afterItem->setToolTip(getToolTip(fileInfo));
                    });
                }
                break;
            }
        };

        // Source 列存"真正的来源类型"，而不是 updateTypes() 的下标：
        // 下标是对话框打开时算出来的，期间若有后台检查完成改了 updateTypes()，
        // accept 时按它去取就会取错来源甚至越界。
        sourceItem->setData(int(type));
        setType(type);
        if(types.size() == 1){
            sourceItem->setText(ModWebsite::toString(type));
            sourceItem->setIcon(ModWebsite::icon(type));
            sourceItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        }else if(types.size() > 1){
            auto comboBox = new QComboBox(this);
            ui->updateTreeView->setIndexWidget(model_.indexFromItem(sourceItem), comboBox);
            for(auto &&type : types)
                comboBox->addItem(ModWebsite::icon(type), ModWebsite::toString(type));
            connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){
                if(index < 0 || index >= types.size()) return;
                auto type = types.at(index);
                sourceItem->setData(int(type));
                setType(type);
            });
        }
    }
}

LocalModUpdateDialog::~LocalModUpdateDialog()
{
    delete ui;
}

void LocalModUpdateDialog::on_LocalModUpdateDialog_accepted()
{
    QList<QPair<LocalMod *, CurseforgeFileInfo> > curseforgeUpdateList;
    QList<QPair<LocalMod *, ModrinthFileInfo> > modrinthUpdateList;
    for(int row = 0; row < model_.rowCount(); row++)
        if(auto item = model_.item(row); item->checkState() == Qt::Checked){
            auto mod = item->data().value<LocalMod*>();
            // Source 列存的是来源类型本身，并做了范围校验；
            // After 列的下标是打开对话框时算的，取之前必须再核一次范围
            // （QList::at 越界是 UB，不是抛异常）。
            auto type = static_cast<ModWebsiteType>(model_.item(row, SourceColumn)->data().toInt());
            auto fileInfoIndex = model_.item(row, AfterColumn)->data().toInt();
            if(type == ModWebsiteType::Curseforge){
                const auto &list = mod->curseforgeUpdater().updateFileInfos();
                if(fileInfoIndex < 0 || fileInfoIndex >= list.size()){
                    qWarning() << "update source changed while dialog was open, skip" << mod->displayName();
                    continue;
                }
                curseforgeUpdateList << QPair<LocalMod *, CurseforgeFileInfo>{ mod, list.at(fileInfoIndex) };
                qDebug() << "Curseforge" << list.at(fileInfoIndex).displayName();
            }
            else if(type == ModWebsiteType::Modrinth){
                const auto &list = mod->modrinthUpdater().updateFileInfos();
                if(fileInfoIndex < 0 || fileInfoIndex >= list.size()){
                    qWarning() << "update source changed while dialog was open, skip" << mod->displayName();
                    continue;
                }
                modrinthUpdateList << QPair<LocalMod *, ModrinthFileInfo>{ mod, list.at(fileInfoIndex) };
                qDebug() << "Modrinth" << list.at(fileInfoIndex).displayName();
            }
        }
    modPath_->updateMods(curseforgeUpdateList, modrinthUpdateList);
}
