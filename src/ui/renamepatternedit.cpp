#include "renamepatternedit.h"

#include <QCompleter>
#include <QStringListModel>
#include <QScrollBar>
#include <QAbstractItemView>
#include <QDebug>

#include "renamehighlighter.h"
#include "tag/tagcategory.h"

RenamePatternEdit::RenamePatternEdit(QWidget *parent):
    QPlainTextEdit(parent),
    highlighter_(new RenameHighlighter(this)),
    completer_(new QCompleter(this))
{
    highlighter_->setDocument(document());
    QStringList stringList{
        "<filename>",
        "<id>",
        "<name>",
        "<version>",
        "<tags|",
        "<capture|",
        "<replace|"
    };
    for(auto &&category : TagCategory::PresetCategories)
        stringList << QString("<tags|%1|>").arg(category.id());
    completer_->setModel(new QStringListModel(stringList, this));
    completer_->popup()->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    completer_->setCompletionMode(QCompleter::PopupCompletion);
    completer_->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(completer_, QOverload<const QString &>::of(&QCompleter::activated), this, &RenamePatternEdit::insertCompletion);
}

void RenamePatternEdit::keyPressEvent(QKeyEvent *e)
{
    if (completer_->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
       switch (e->key()) {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
       default:
           break;
       }
    } else{
        //no new line
        if(e->key() == Qt::Key_Return || e->key() == Qt::Key_Tab)
            return;
    }
    QPlainTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
                             e->modifiers().testFlag(Qt::ShiftModifier);
    if (ctrlOrShift && e->text().isEmpty())
        return;

    QString completionPrefix = textUnderCursor();

    if (completionPrefix != completer_->completionPrefix()) {
        completer_->setCompletionPrefix(completionPrefix);
        completer_->popup()->setCurrentIndex(completer_->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(qMax(200, completer_->popup()->sizeHintForColumn(0)
                + completer_->popup()->verticalScrollBar()->sizeHint().width()));
    completer_->complete(cr);
}

void RenamePatternEdit::focusInEvent(QFocusEvent *e)
{
    completer_->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

void RenamePatternEdit::insertCompletion(const QString &completion)
{
    if (completer_->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - completer_->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

QString RenamePatternEdit::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.clearSelection();
    tc.movePosition(QTextCursor::WordLeft,QTextCursor::MoveAnchor, 1);
    tc.select(QTextCursor::WordUnderCursor);
    auto str =  tc.selectedText();
    tc.clearSelection();
    tc.select(QTextCursor::WordUnderCursor);
    tc.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    auto str2 = tc.selectedText();
    qDebug() << "selected:" << str << str2;
    if(!str.isEmpty() && str.right(1) == "<")
        return str.right(1);
    if(!str2.isEmpty()){
        if(str2.endsWith("<tags|")){
            str.prepend(str == "|"? "<tags" : "<tags|");
        }
        else if(str2.right(1) == "<")
            str.prepend(str2.right(1));
    }
    qDebug() << "result:" << str;
    return str;
}
