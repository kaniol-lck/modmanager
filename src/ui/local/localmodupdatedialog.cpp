#include "localmodupdatedialog.h"
#include "ui_localmodupdatedialog.h"

#include "local/localmodpath.h"
#include "local/localmod.h"
#include "local/localfilelinker.h"

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

    for(auto &&map : modPath_->modMaps())
        for(const auto &mod : map){
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
            afterItem->setForeground(Qt::darkGreen);
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
                        comboBox->setStyleSheet("color: darkgreen");
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
                        comboBox->setStyleSheet("color: darkgreen");
                        ui->updateTreeView->setIndexWidget(model_.indexFromItem(afterItem), comboBox);
                        for(const auto &fileInfo : mod->modrinthUpdater().updateFileInfos())
                            comboBox->addItem(fileInfo.displayName());
                        afterItem->setToolTip(getToolTip(mod->curseforgeUpdater().updateFileInfos().first()));
                        connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){
                            auto &fileInfo = mod->modrinthUpdater().updateFileInfos().at(index);
                            afterItem->setData(index);
                            afterItem->setToolTip(getToolTip(fileInfo));
                        });
                    }
                    break;
                }
            };

            sourceItem->setData(0);
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
                    auto type = types.at(index);
                    sourceItem->setData(index);
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
            auto type = mod->updateTypes().at(model_.item(row, SourceColumn)->data().toInt());
            auto fileInfoIndex = model_.item(row, AfterColumn)->data().toInt();
            if(type == ModWebsiteType::Curseforge){
                curseforgeUpdateList << QPair{ mod, mod->curseforgeUpdater().updateFileInfos().at(fileInfoIndex) };
                qDebug() << "Curseforge" << mod->curseforgeUpdater().updateFileInfos().at(fileInfoIndex).displayName();
            }
            else if(type == ModWebsiteType::Modrinth){
                modrinthUpdateList << QPair{ mod, mod->modrinthUpdater().updateFileInfos().at(fileInfoIndex) };
                qDebug() << "Modrinth" << mod->modrinthUpdater().updateFileInfos().at(fileInfoIndex).displayName();
            }
        }
    modPath_->updateMods(curseforgeUpdateList, modrinthUpdateList);
}
