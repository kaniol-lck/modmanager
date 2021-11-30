#include "batchrenamedialog.h"
#include "ui_batchrenamedialog.h"

#include <QMenu>
#include <QCompleter>
#include <QScrollBar>
#include <QStringListModel>

#include "local/localmod.h"
#include "local/localmodpath.h"
#include "ui/renamehighlighter.h"

BatchRenameDialog::BatchRenameDialog(QWidget *parent, QList<LocalMod *> mods) :
    QDialog(parent),
    ui(new Ui::BatchRenameDialog)
{
    ui->setupUi(this);
    ui->widget->setVisible(false);
    ui->renameTreeView->setModel(&model_);
    ui->renameTreeView->header()->setSectionResizeMode(QHeaderView::Stretch);

    auto menu = new QMenu(this);
    auto getSubmenu = [=](const QString &title, const QString &category){
        auto subMenu = new QMenu(title, this);
        connect(subMenu->addAction(tr("None")), &QAction::triggered, this, [=]{
            ui->renamePattern->insertPlainText("<tags|" + category + "|%1>");
        });
        for(const auto &str : { "[%1]", "(%1)", "【%1】" })
        connect(subMenu->addAction(str), &QAction::triggered, this, [=]{
            ui->renamePattern->insertPlainText("<tags|" + category + "|" + str + ">");
        });
        return subMenu;
    };
    menu->addMenu(getSubmenu(tr("All tags"), ""));
    menu->addMenu(getSubmenu(tr("Type tag"), "type"));
    menu->addMenu(getSubmenu(tr("Functionality tag"), "functionality"));
    menu->addMenu(getSubmenu(tr("Translation tag"), "translation"));
    menu->addMenu(getSubmenu(tr("Custom tag"), "custom"));
    ui->addTagsButton->setMenu(menu);

    auto historyMenu = new QMenu(this);
    auto list = Config().getRenamePatternHistory();
    historyMenu->addSection(tr("Rename Histories"));
    for(auto &&v : list){
        auto str = v.toString();
        historyMenu->addAction(str, this, [=]{
            ui->renamePattern->setPlainText(str);
        });
    }
    ui->historyButton->setMenu(historyMenu);

    ui->renamePattern->setPlainText("<filename>");
    setMods(mods);
}

BatchRenameDialog::BatchRenameDialog(QWidget *parent, LocalModPath *modPath) :
    BatchRenameDialog(parent)
{
    auto updateModList = [=]{
        QList<LocalMod *> mods;
        for(auto &&map : modPath->modMaps())
            for(const auto &mod : map)
                mods << mod;
        setMods(mods);
    };
    updateModList();
    connect(modPath, &LocalModPath::modListUpdated, this, updateModList);
}

BatchRenameDialog::~BatchRenameDialog()
{
    delete ui;
}

void BatchRenameDialog::setMods(QList<LocalMod *> mods)
{
    modList_.clear();
    fileNameList_.clear();
    model_.clear();
    model_.setHorizontalHeaderItem(NameColumn, new QStandardItem(tr("Mod Name")));
    model_.setHorizontalHeaderItem(OldFileNameColumn, new QStandardItem(tr("Old File Name")));
    model_.setHorizontalHeaderItem(NewFileNameColumn, new QStandardItem(tr("New File Name")));
    ui->renameTreeView->header()->setSectionResizeMode(NameColumn, QHeaderView::Fixed);
    ui->renameTreeView->setColumnWidth(NameColumn, 250);
    for(const auto &mod : mods){
        auto enabled = !mod->isDisabled();

        auto nameItem = new QStandardItem();
        nameItem->setText(mod->commonInfo()->name());
        nameItem->setCheckable(enabled);
        nameItem->setCheckState(enabled? Qt::Checked : Qt::Unchecked);
        nameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        nameItem->setEditable(false);
        nameItem->setEnabled(enabled);

        auto oldFilename = mod->modFile()->fileInfo().fileName();
        auto beforeItem = new QStandardItem();
        beforeItem->setText(oldFilename);
        if(enabled) beforeItem->setForeground(Qt::darkRed);
        beforeItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        beforeItem->setEditable(false);
        beforeItem->setEnabled(enabled);

        auto newFileName = oldFilename;
        auto afterItem = new QStandardItem();
        afterItem->setText(newFileName);
        if(enabled) afterItem->setForeground(Qt::darkGreen);
        afterItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        afterItem->setEditable(false);
        afterItem->setEnabled(enabled);

        modList_ << mod;
        fileNameList_ << newFileName;
        model_.appendRow({nameItem, beforeItem, afterItem});
    }
    on_renamePattern_textChanged();
}

void BatchRenameDialog::on_BatchRenameDialog_accepted()
{
    Config config;
    auto list = config.getRenamePatternHistory();
    auto str = ui->renamePattern->toPlainText();
    if(list.contains(str))
        list.removeAll(str);
    list.prepend(str);
    config.setRenamePatternHistory(list);
    for(int row = 0; row < model_.rowCount(); row++)
        if(model_.item(row)->checkState() == Qt::Checked)
            modList_[row]->modFile()->rename(fileNameList_.at(row));
}

void BatchRenameDialog::on_toolButton_toggled(bool checked)
{
    ui->widget->setVisible(checked);
}

void BatchRenameDialog::on_toolButton_2_clicked()
{
    ui->renamePattern->insertPlainText("<filename>");
}

void BatchRenameDialog::on_toolButton_3_clicked()
{
    ui->renamePattern->insertPlainText("<id>");
}

void BatchRenameDialog::on_toolButton_4_clicked()
{
    ui->renamePattern->insertPlainText("<name>");
}

void BatchRenameDialog::on_toolButton_5_clicked()
{
    ui->renamePattern->insertPlainText("<version>");
}

void BatchRenameDialog::on_renamePattern_textChanged()
{
    auto tagConcat = [=](QList<Tag> tags, QString replacer = ""){
        if(!replacer.contains("%1")) replacer = "[%1]";
        QStringList tagList;
        for(const auto &tag : tags)
            tagList << replacer.arg(tag.name());
        return tagList.join("");
    };
    bool ok = true;
    for(int row = 0; row < model_.rowCount(); row++){
        auto item = model_.item(row, NewFileNameColumn);
        auto mod = modList_.at(row);
        auto newFileName = ui->renamePattern->toPlainText();
        newFileName.replace("<filename>", mod->modFile()->fileInfo().completeBaseName());
        newFileName.replace("<name>", mod->commonInfo()->name());
        newFileName.replace("<id>", mod->commonInfo()->id());
        newFileName.replace("<version>", mod->commonInfo()->version());
        newFileName.replace("<tags>", tagConcat(mod->tags()));
        //tags
        auto it = QRegularExpression(R"(<tags\|(.+?)>)").globalMatch(newFileName);
        while (it.hasNext()) {
            auto match = it.next();
            auto stringList = match.captured(1).split("|");
            if(stringList.size() == 0) continue;
            QList<TagCategory> categories;
            if(stringList.first().isEmpty())
                categories = TagCategory::PresetCategories;
            else
                categories << TagCategory::fromId(stringList.first());
            QString replacer = stringList.size() >= 2? stringList.at(1) : "";
            newFileName.replace("<tags|" + match.captured(1) + ">", tagConcat(mod->tagManager().tags(categories), replacer));
        }
        //filename capture-replace
        it = QRegularExpression(R"(<capture\|(.+?)>)").globalMatch(newFileName);
        while (it.hasNext()) {
            auto match = it.next();
            auto stringList = match.captured(1).split("|");
            if(stringList.size() < 2) continue;
            auto replacedStr = stringList.at(0);
            auto replacee = stringList.at(1);
            auto replacer = stringList.size() > 2? stringList.at(2) : "\\1";
            auto it2 = QRegularExpression(replacee).globalMatch(replacedStr);
            QString result;
            while(it2.hasNext()){
                auto match2 = it2.next();
                auto str = match2.captured();
                str.replace(QRegularExpression(replacee), replacer);
                result += str;
            }
            newFileName.replace("<capture|" + match.captured(1) + ">", result);
        }
        //replace
        it = QRegularExpression(R"(<replace\|(.+?)>)").globalMatch(newFileName);
        while (it.hasNext()) {
            auto match = it.next();
            auto stringList = match.captured(1).split("|");
            if(stringList.size() != 3) continue;
            auto replacedStr = stringList.at(0);
            auto replacee = stringList.at(1);
            auto replacer = stringList.at(2);
            replacedStr.replace(QRegularExpression(replacee), replacer);
            newFileName.replace("<replace|" + match.captured(1) + ">", replacedStr);
        }

        if(newFileName.isEmpty()) ok = false;
        fileNameList_[row] = newFileName;
        item->setText(newFileName + "." + mod->modFile()->fileInfo().suffix());
    }
    if(ok && QStringList(fileNameList_).removeDuplicates())
        ok = false;
    ui->statusText->setVisible(!ok);
    ui->buttonBox->setDisabled(!ok);
}
