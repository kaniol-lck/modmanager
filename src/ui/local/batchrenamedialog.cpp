#include "batchrenamedialog.h"
#include "ui_batchrenamedialog.h"

#include <QSyntaxHighlighter>

#include "local/localmod.h"
#include "local/localmodpath.h"

BatchRenameDialog::BatchRenameDialog(QWidget *parent, LocalModPath *modPath) :
    QDialog(parent),
    ui(new Ui::BatchRenameDialog),
    modPath_(modPath)
{
    ui->setupUi(this);
    ui->tableView->setModel(&model_);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //TODO: customize
    ui->renamePattern->setText("<filename>");

    model_.setHorizontalHeaderItem(NameColumn, new QStandardItem(tr("Mod Name")));
    model_.setHorizontalHeaderItem(OldFileNameColumn, new QStandardItem(tr("Old File Name")));
    model_.setHorizontalHeaderItem(NewFileNameColumn, new QStandardItem(tr("New File Name")));

    ui->tableView->horizontalHeader()->setSectionResizeMode(NameColumn, QHeaderView::Fixed);
    ui->tableView->setColumnWidth(NameColumn, 200);

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
        model_.appendRow({nameItem, beforeItem, afterItem});
    }
}

BatchRenameDialog::~BatchRenameDialog()
{
    delete ui;
}

void BatchRenameDialog::on_renamePattern_textChanged(const QString &arg1)
{
    auto tagConcat = [=](QList<Tag> tags, QString replacer = ""){
        if(!replacer.contains("%1")) replacer = "[%1]";
        QStringList tagList;
        for(const auto &tag : tags)
            tagList << replacer.arg(tag.name());
        return tagList.join("");
    };
    for(int row = 0; row < model_.rowCount(); row++){
        auto item = model_.item(row, NewFileNameColumn);
        auto mod = modList_.at(row);
        auto newFileName = arg1;
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
            auto category = TagCategory::fromId(stringList.first());
            QString replacer = stringList.size() >= 2? stringList.at(1) : "";
            newFileName.replace("<tags|" + rx.cap(1) + ">", tagConcat(mod->tagManager().tags({category}), replacer));
            pos += rx.matchedLength();
        }
        item->setText(newFileName + "." + mod->modFile()->fileInfo().suffix());
    }
}

