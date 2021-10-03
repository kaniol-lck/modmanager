#ifndef RENAMEHIGHLIGHTER_H
#define RENAMEHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class RenameHighlighter : public QSyntaxHighlighter
{
public:
    explicit RenameHighlighter(QTextDocument *parent = nullptr);
    void highlightBlock(const QString &text);
};

#endif // RENAMEHIGHLIGHTER_H
