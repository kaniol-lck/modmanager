#ifndef MMLOGGER_H
#define MMLOGGER_H

#include <QString>
#include <QTextStream>
#include <QUrl>

class MMLogger
{
public:
    MMLogger(const QString &context = QString(), const QString &field = QString(), bool mute = false);
    ~MMLogger();

    static MMLogger dtor(const QString &field = QString());
    static MMLogger dtor(QObject *object);

    static MMLogger network(const QString &field = QString());
    static MMLogger network(QObject *object);

    virtual MMLogger &operator<<(const QUrl &url);;
    template<typename T>
    MMLogger &operator<<(T output)
    {
#ifdef QT_DEBUG
        if(!mute_)
            ts_ << output << " ";
#endif //QT_DEBUG
        return *this;
    }
private:
    QTextStream ts_;
    QString context_;
    QString field_;
    bool mute_ = false;
};

#endif // MMLOGGER_H
