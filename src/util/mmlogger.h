#ifndef MMLOGGER_H
#define MMLOGGER_H

#include <QString>
#include <QTextStream>

class MMLogger
{
public:
    MMLogger(const QString &context = QString(), const QString &field = QString());
    ~MMLogger();

    static MMLogger dector(const QString &field = QString());
    static MMLogger dector(QObject *object);

    template<typename T>
    MMLogger &operator<<(T output)
    {
#ifdef QT_DEBUG
        ts_ << output << " ";
#endif //QT_DEBUG
        return *this;
    }
private:
    QTextStream ts_;
    QString context_;
    QString field_;
};

#endif // MMLOGGER_H
