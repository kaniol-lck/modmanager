#include "renamehighlighter.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>

#include "tag/tagcategory.h"

RenameHighlighter::RenameHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{}

void RenameHighlighter::highlightBlock(const QString &text)
{
    //bracket
    auto i = QRegularExpression("<|>").globalMatch(text);
    while (i.hasNext()){
        QTextCharFormat format;
        format.setForeground(Qt::blue);
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), format);
    }
    //main pattern
    for(auto &&str : QStringList{ "filename", "id", "version", "name", "tags", "replace" }){
        i = QRegularExpression("<(" + str + ")(\\||>)").globalMatch(text);
        while (i.hasNext()){
            QTextCharFormat format;
            format.setForeground(Qt::darkGreen);
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart(1), match.capturedLength(1), format);
        }
    }
    //tag category
    for(const auto &category : TagCategory::PresetCategories){
        i = QRegularExpression("<tags\\|(" + category.id() + ")?(\\|(.+?))?>").globalMatch(text);
        while (i.hasNext()){
            QTextCharFormat format1;
            format1.setForeground(Qt::darkCyan);
            format1.setFontItalic(true);
            QTextCharFormat format2;
            format2.setForeground(Qt::darkYellow);
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart(1), match.capturedLength(1), format1);
            setFormat(match.capturedStart(3), match.capturedLength(3), format2);
        }
    }
    //filename/replace
    for(auto &&str : QStringList{ "filename", "replace" }){
        i = QRegularExpression("<" + str + "\\|(.+?)>").globalMatch(text);
        while (i.hasNext()){
            QTextCharFormat format1;
            format1.setForeground(Qt::darkCyan);
            format1.setFontItalic(true);
            QTextCharFormat format2;
            format2.setForeground(Qt::darkYellow);
            QRegularExpressionMatch match = i.next();
            int capturedStart = match.capturedStart(1);
            for(auto &&s : match.captured(1).split("|")){
                setFormat(capturedStart, s.length(), format2);
                capturedStart += s.length() + 1;
            }
        }
    }
    //%1 placeholder
    i = QRegularExpression("%1").globalMatch(text);
    while (i.hasNext()){
        QTextCharFormat format;
        format.setForeground(Qt::darkMagenta);
        QRegularExpressionMatch match = i.next();
        setFormat(match.capturedStart(), match.capturedLength(), format);
    }
}
