#include "modfilerenamer.h"

#include "local/localmod.h"

ModFileRenamer::ModFileRenamer(const QString &renamePattern) :
    renamePattern_(renamePattern)
{}

const QString &ModFileRenamer::renamePattern() const
{
    return renamePattern_;
}

void ModFileRenamer::setRenamePattern(const QString &renamePattern)
{
    renamePattern_ = renamePattern;
}

QString ModFileRenamer::newBaseFileName(LocalMod *mod)
{
    auto tagConcat = [=](QList<Tag> tags, QString replacer = ""){
        if(!replacer.contains("%1")) replacer = "[%1]";
        QStringList tagList;
        for(const auto &tag : tags)
            tagList << replacer.arg(tag.name());
        return tagList.join("");
    };

    auto newFileName = renamePattern_;
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
    return newFileName;
}

void ModFileRenamer::renameMod(LocalMod *mod)
{
    if(auto fileName = newBaseFileName(mod); !fileName.isEmpty())
        mod->modFile()->rename(newBaseFileName(mod));
}

void ModFileRenamer::renameMods(QList<LocalMod *> mods)
{
    for(auto mod : mods)
        renameMod(mod);
}
