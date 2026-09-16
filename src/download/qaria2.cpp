#include "qaria2.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QTimer>
#include <QDebug>
#include <QtConcurrent>
#include <QMessageBox>

#include "qaria2downloader.h"
#include "util/mmlogger.h"
#include "config.hpp"

int QAria2::downloadEventCallback(aria2::Session *session[[maybe_unused]], aria2::DownloadEvent event, const aria2::A2Gid gid, void *userData[[maybe_unused]])
{
    // ⚠️ 这个回调是在 aria2 的工作线程里被调用的（见构造函数里的 QtConcurrent::run(aria2::run)）。
    // 原实现直接在这里遍历 downloaders_ 并调用 downloader->setEvent()，
    // 于是 setEvent -> update/finished 一路都在非 GUI 线程里跑：
    // 更新流程的回调会去动 model、菜单、文件与界面控件，既是数据竞争（downloaders_ / info_ / status_
    // 都被两个线程同时读写），也是明确的 Qt 线程误用。
    // 这里只把事件投递回本对象所在线程，downloaders_ 的查找也挪到那边做。
    QMetaObject::invokeMethod(qaria2(), [event, gid]{
        qaria2()->dispatchDownloadEvent(event, gid);
    }, Qt::QueuedConnection);
    return 0;
}

void QAria2::dispatchDownloadEvent(int event, qulonglong gid)
{
    // setEvent() 会同步发出 finished / downloadFailed / statusChanged，
    // 这些槽里可能又去发起新的下载（从而修改 downloaders_）。
    // 先拷一份再遍历，避免"遍历途中列表被改"。
    const auto downloaders = downloaders_;
    for(auto &&downloader : downloaders)
        if(downloader->gid_ == gid){
            downloader->setEvent(static_cast<aria2::DownloadEvent>(event));
            return;
        }
    qDebug() << "download event for unknown gid" << gid << "event" << event;
}

// OpenSSL 3 把 provider（legacy / default）做成**运行时才 LoadLibrary 的独立模块**，
// 由 OSSL_PROVIDER_load() 按 OPENSSL_MODULES 环境变量、或编译期写死的 MODULESDIR 去找。
// aria2 的 Platform::setUp() 在 OpenSSL 3.x 下会无条件加载 legacy provider，
// 加载不到就抛异常 ⇒ aria2::libraryInit() 返回 -1 ⇒ 用户一启动就看到"下载功能可能不可用"。
//
// 而那两个路径在分发包里都不成立：
//   * MSYS2 给 libcrypto 编进去的是 MSYS 风格的 "/mingw64/lib/ossl-modules"，
//     Windows 上会被当成"当前盘符根下的 mingw64\lib\ossl-modules" ⇒ 必然不存在；
//   * Homebrew 编进去的是它自己的 /usr/local/opt/openssl@3/lib/ossl-modules，
//     用户机器上没装 brew 的 openssl@3 就同样不存在。
// 所以把搜索路径显式指到随程序分发的 ossl-modules 目录。
//
// 两个要点：
//   1. 只在用户/发行版没有显式设置 OPENSSL_MODULES 时接管，别覆盖别人的配置；
//   2. 目录不存在就什么都不做 —— 否则会把"系统本来能用的 provider 路径"指成空目录，
//      在 Linux（libssl3 装在 /usr/lib/x86_64-linux-gnu/ossl-modules）上反而弄坏。
static void ensureOpensslModulesPath()
{
    if(!qEnvironmentVariableIsEmpty("OPENSSL_MODULES"))
        return;
    const QString dir = QCoreApplication::applicationDirPath() + QStringLiteral("/ossl-modules");
    if(!QFileInfo::exists(dir))
        return;
    qputenv("OPENSSL_MODULES", QDir::toNativeSeparators(dir).toLocal8Bit());
    MMLogger() << "OPENSSL_MODULES set to" << dir;
}

QAria2::QAria2(QObject *parent) : QObject(parent)
{
    ensureOpensslModulesPath();
    if(auto code = aria2::libraryInit())
        QMessageBox::warning(0, tr("Aria2 Error"), tr("Aria2 downloader init failed, download function may not work.") +
                             "\n" + tr("Error code: %1").arg(code));
    config_.downloadEventCallback = downloadEventCallback;
    config_.keepRunning = true;
    aria2::KeyVals options;
    options.emplace_back(std::make_pair("check-certificate", "false"));
    session_ = aria2::sessionNew(options, config_);
    QtConcurrent::run([=]{
        aria2::run(session_, aria2::RUN_DEFAULT);
    });
    MMLogger() << "aria2 start running";
    //interval between refreshing info
    timer_.start(1000);
    connect(&timer_, &QTimer::timeout, this, [=]{
        aria2::GlobalStat gstat = aria2::getGlobalStat(session_);
        emit downloadSpeed(gstat.downloadSpeed, gstat.uploadSpeed);
        emit globalDownloadStat(gstat.numWaiting, gstat.numActive, gstat.downloadSpeed, gstat.uploadSpeed);
        // 只轮询还没结束的任务。downloaders_ 只增不减（已完成的下载要留在下载列表里），
        // 不跳过终态的话，用久了就是每秒对几十上百个已完成任务反复取句柄、发信号。
        // 同样先拷一份：update() 会同步发信号，槽里可能改动 downloaders_。
        const auto downloaders = downloaders_;
        for (auto &&downloader : downloaders)
            if(!downloader->isTerminal())
                downloader->update();
    });
    connect(qApp, &QCoreApplication::aboutToQuit, this, [=]{
        aria2::shutdown(session_);
        MMLogger() << "aria2 shutdown.";
        // FIXME: sometime app donot exit after close
        // (when local mod linked)
        qTerminate();
    });
    updateOptions();
}

QAria2::~QAria2()
{
    for(const auto &downloader : qAsConst(downloaders_))
        downloader->deleteDownloadHandle();
    downloaders_.clear();
    aria2::sessionFinal(session_);
    aria2::libraryDeinit();
}

QAria2 *QAria2::qaria2()
{
    static QAria2 qaria2;
    return &qaria2;
}

void QAria2::updateOptions()
{
    Config config;
    aria2::KeyVals options;
    options.emplace_back("timeout", std::to_string(config.getAria2timeout()));
    options.emplace_back("max-tries", std::to_string(config.getAria2maxTries()));
    options.emplace_back("max-concurrent-downloads", std::to_string(config.getAria2maxConcurrentDownloads()));
    aria2::changeGlobalOption(session_, options);
}

const QList<QAria2Downloader *> &QAria2::downloaders() const
{
    return downloaders_;
}

aria2::Session *QAria2::session() const
{
    return session_;
}

QAria2Downloader *QAria2::downloadNoRedirect(const QUrl &url, const QString &path)
{
    auto downloader = new QAria2Downloader(DownloadFileInfo(url, path));
    return download(downloader);
}

QAria2Downloader *QAria2::download(const QUrl &url, const QString &path)
{
    auto downloader = new QAria2Downloader(DownloadFileInfo(url, path));
    //handle redirect
    downloader->handleRedirect();
    connect(downloader, &AbstractDownloader::redirected, this, [=]{
        download(downloader);
    });
    return downloader;
}

QAria2Downloader *QAria2::download(QAria2Downloader *downloader)
{
    // 同一个 downloader 只登记一次。去重必须放在 addUri 之前：
    // 重复调用（例如 redirect 回调被触发两次）会往 aria2 里再塞一个同地址的任务，
    // 并把 gid_ 覆盖成新的，旧任务就此失联 —— 它的事件回来时按 gid 找不到主人。
    bool registered = false;
    for(auto &&d : qAsConst(downloaders_))
        if(d == downloader){
            registered = true;
            break;
        }
    if(registered){
        // 放在循环外面调用：update() 会同步发信号，在遍历中调用有被改列表的风险。
        downloader->update();
        return downloader;
    }
    std::vector<std::string> urls = { downloader->info().url().toString().toStdString() };
    aria2::KeyVals options;
    options.emplace_back("dir", downloader->info().path().toStdString());
    options.emplace_back("continue", "false");
    aria2::A2Gid gid = 0;
    int rv = aria2::addUri(session_, &gid, urls, options);
    if(rv < 0) {
        // addUri 失败（地址为空 / 参数非法 / 会话已关闭）：aria2 里根本没有这个任务，
        // 也就不可能再有任何事件。原实现只是 qDebug 一下接着往下走，还会在后面的循环里
        // 因为 gid == 0（未登记的 downloader 初始值也是 0）而直接 return nullptr ——
        // 调用方永远等不到 finished / downloadFailed，更新流程就此卡死在"更新中"。
        MMLogger("Download", "error") << "addUri failed:" << rv << downloader->info().url().toString();
        // 不能在这里直接发 downloadFailed()：调用方是在本函数返回之后才 connect 的。
        // reportFailure() 内部走队列调用，把失败报告推迟到事件循环的下一轮。
        downloader->reportFailure();
        return downloader;
    }
    downloader->setGid(gid);
    downloaders_ << downloader;
    downloader->update();
    emit downloaderAdded(downloader);
    return downloader;
}
