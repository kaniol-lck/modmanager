#ifndef REPLY_HPP
#define REPLY_HPP

#include <QNetworkReply>
#include <QEventLoop>

template<typename ... Result>
class Reply
{
public:
    Reply(QNetworkReply *reply, std::function<std::tuple<Result...> ()> resultInterpreter = {}) :
        loop_(new QEventLoop),
        reply_(reply),
        resultInterpreter_(resultInterpreter)
    {
        QObject::connect(reply_, &QNetworkReply::finished, loop_, &QEventLoop::quit);
    }

    Reply(Reply &&other) :
        loop_(other.loop_),
        reply_(other.reply_),
        resultInterpreter_(other.resultInterpreter_)
    {
        other.loop_ = nullptr;
        other.reply_ = nullptr;
        other.resultInterpreter_ = {};
    }

    inline bool runBackground() const
    {
        return runBackground_;
    }

    inline void setRunBackground(bool newRunBackground)
    {
        runBackground_ = newRunBackground;
    };

    Reply(const Reply &other) = delete;

    ~Reply()
    {
        if(loop_) loop_->deleteLater();
    }

    std::shared_ptr<Reply<Result...>> asShared()
    {
        return std::make_shared<Reply<Result...>>(std::move(*this));
    }

    std::unique_ptr<Reply<Result...>> asUnique()
    {
        return std::make_unique<Reply<Result...>>(std::move(*this));
    }

    QNetworkReply *reply() const
    {
        return reply_;
    }

    bool isRunning() const
    {
        return reply_? reply_->isRunning() : false;
    }

    void waitForFinished()
    {
        loop_->exec();
    }

    void setInterpreter(std::function<std::tuple<Result...> ()> func){
        resultInterpreter_ = func;
    };

    template<typename Func1>
    void setOnFinished(Func1 callback){
        //copy-capture to prevent this deconstructed
        QObject::connect(reply_, &QNetworkReply::finished, [=, reply = reply_, resultInterpreter = resultInterpreter_, runBackground = runBackground_]{
            if(reply->error() != QNetworkReply::NoError) {
                qDebug() << "error!!";
                qDebug() << reply->errorString();
            } else
                std::apply(callback, resultInterpreter());
            reply->deleteLater();
            if(!runBackground) reply_ = nullptr;
        });
    };

    template<typename Func1, typename Func2>
    void setOnFinished(Func1 callback, Func2 errorHandler){
        //copy-capture to prevent this deconstructed
        QObject::connect(reply_, &QNetworkReply::finished, [=, reply = reply_, resultInterpreter = resultInterpreter_, runBackground = runBackground_]{
            if(reply->error() != QNetworkReply::NoError) {
                qDebug() << "error!!";
                qDebug() << reply->errorString();
                errorHandler();
            } else
                std::apply(callback, resultInterpreter());
            reply->deleteLater();
            if(!runBackground) reply_ = nullptr;
        });
    };

private:
    QEventLoop *loop_;
    QNetworkReply *reply_ = nullptr;
    std::function<std::tuple<Result...> ()> resultInterpreter_;
    bool runBackground_ = false;
};

#endif // REPLY_HPP
