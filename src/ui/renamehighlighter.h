#ifndef RENAMEHIGHLIGHTER_H
#define RENAMEHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class RenameHighlighter : public QSyntaxHighlighter
{
public:
    explicit RenameHighlighter(QObject *parent);
    explicit RenameHighlighter(QTextDocument *parent = nullptr);
protected:
    void highlightBlock(const QString &text) override;
};

#endif // RENAMEHIGHLIGHTER_H
