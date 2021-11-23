#ifndef RENAMEPATTERNEDIT_H
#define RENAMEPATTERNEDIT_H

#include <QPlainTextEdit>

class QCompleter;
class RenameHighlighter;
class RenamePatternEdit : public QPlainTextEdit
{
public:
    RenamePatternEdit(QWidget *parent);
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;
private slots:
    void insertCompletion(const QString &completion);
private:
    QString textUnderCursor() const;
private:
    RenameHighlighter *highlighter_;
    QCompleter *completer_;
};

#endif // RENAMEPATTERNEDIT_H
