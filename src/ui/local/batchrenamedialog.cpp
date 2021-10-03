#include "batchrenamedialog.h"
#include "ui_batchrenamedialog.h"

#include <QMenu>

#include "local/localmod.h"
#include "local/localmodpath.h"
#include "ui/renamehighlighter.h"

BatchRenameDialog::BatchRenameDialog(QWidget *parent, LocalModPath *modPath) :
    QDialog(parent),
    ui(new Ui::BatchRenameDialog),
    modPath_(modPath)
{
    ui->setupUi(this);
    ui->widget->setVisible(false);
    ui->tableView->setModel(&model_);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->renamePattern->installEventFilter(this);
    highlighter_ = new RenameHighlighter(ui->renamePattern->document());

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

    //TODO: customize
    ui->renamePattern->setPlainText("<filename>");
    //TODO: syntax highlight

    updateModList();
    connect(modPath_, &LocalModPath::modListUpdated, this, &BatchRenameDialog::updateModList);
}

BatchRenameDialog::~BatchRenameDialog()
{
    delete ui;
}

bool BatchRenameDialog::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == ui->renamePattern && e->type() == QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
        int keyValue = keyEvent->key();
        if(keyValue == Qt::Key_Return || keyValue == Qt::Key_Tab)
            return true;
    }
    return QDialog::eventFilter(obj, e);
}

void BatchRenameDialog::updateModList()
{
    modList_.clear();
    fileNameList_.clear();
    model_.clear();
    model_.setHorizontalHeaderItem(NameColumn, new QStandardItem(tr("Mod Name")));
    model_.setHorizontalHeaderItem(OldFileNameColumn, new QStandardItem(tr("Old File Name")));
    model_.setHorizontalHeaderItem(NewFileNameColumn, new QStandardItem(tr("New File Name")));
    ui->tableView->horizontalHeader()->setSectionResizeMode(NameColumn, QHeaderView::Fixed);
    ui->tableView->setColumnWidth(NameColumn, 250);
    for(const auto &mod : modPath_->modMap()){
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
        QRegExp rx(R"(<tags\|(.+)>)");
        rx.setMinimal(true);
        int pos = 0;
        auto str = newFileName;
        while ((pos = rx.indexIn(str, pos)) != -1) {
            auto stringList = rx.cap(1).split("|");
            if(stringList.size() == 0) continue;
            QList<TagCategory> categories;
            if(stringList.first().isEmpty())
                categories = TagCategory::PresetCategories;
            else
                categories << TagCategory::fromId(stringList.first());
            QString replacer = stringList.size() >= 2? stringList.at(1) : "";
            newFileName.replace("<tags|" + rx.cap(1) + ">", tagConcat(mod->tagManager().tags(categories), replacer));
            pos += rx.matchedLength();
        }
        if(newFileName.isEmpty()) ok = false;
        fileNameList_[row] = newFileName;
        item->setText(newFileName + "." + mod->modFile()->fileInfo().suffix());
    }
    if(auto list = fileNameList_; list.removeDuplicates())
        ok = false;
    ui->statusText->setVisible(!ok);
    ui->buttonBox->setDisabled(!ok);
}
