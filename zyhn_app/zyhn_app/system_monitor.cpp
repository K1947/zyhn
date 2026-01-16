#include "system_monitor.h"
#include <QDebug>
#include <QRegularExpression>

SystemMonitor::SystemMonitor(QObject *parent)
    : QObject(parent)
    , timer_(new QTimer(this))
    , isMonitoring_(false)
    , lastCpuUsage_(0.0)
    , lastCpuTime_(0)
    , lastSystemTime_(0)
    , lastMemoryUsage_(0.0)
    , lastGpuUsage_(-1)
{
    process_ = new QProcess();
    connect(timer_, &QTimer::timeout, this, &SystemMonitor::updateSystemInfo);
    connect(process_, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &SystemMonitor::onProcessFinished);
    
    memset(&lastIdleTime_, 0, sizeof(FILETIME));
    memset(&lastKernelTime_, 0, sizeof(FILETIME));
    memset(&lastUserTime_, 0, sizeof(FILETIME));
}

SystemMonitor::~SystemMonitor()
{
    stopMonitoring();
}

void SystemMonitor::startMonitoring(int intervalMs)
{
    if (!isMonitoring_) {
        timer_->start(intervalMs);
        isMonitoring_ = true;
        qDebug() << "SystemMonitor: 开始监控系统信息，间隔" << intervalMs << "ms";
    }
}

void SystemMonitor::stopMonitoring()
{
    if (isMonitoring_) {
        timer_->stop();
        isMonitoring_ = false;
        qDebug() << "SystemMonitor: 停止监控系统信息";

        if (process_->state() == QProcess::Running) {
            process_->kill();
            process_->deleteLater();
        }
    }
}

bool SystemMonitor::isMonitoring() const
{
    return isMonitoring_;
}


double SystemMonitor::getCpuUsage() const
{
    return lastCpuUsage_;
}

double SystemMonitor::getMemoryUsage() const
{
    return lastMemoryUsage_;
}

int SystemMonitor::getGpuUsage() const
{
    return lastGpuUsage_;
}

QString SystemMonitor::getCurrentTime() const
{
    return formatCurrentTime();
}

void SystemMonitor::updateSystemInfo()
{
    double cpuUsage = calculateCpuUsage();
    double memoryUsage = calculateMemoryUsage();
    calculateGpuUsage();
    QString currentTime = formatCurrentTime();
    
    lastCpuUsage_ = cpuUsage;
    lastMemoryUsage_ = memoryUsage;    
    emit systemInfoUpdated(cpuUsage, memoryUsage, currentTime);
}

QString SystemMonitor::formatCurrentTime() const
{
    QDateTime now = QDateTime::currentDateTime();
    return now.toString("yyyy-MM-dd hh:mm:ss");
}

double SystemMonitor::calculateCpuUsage()
{
    FILETIME idleTime, kernelTime, userTime;
    
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        ULARGE_INTEGER idle, kernel, user;
        idle.LowPart = idleTime.dwLowDateTime;
        idle.HighPart = idleTime.dwHighDateTime;
        kernel.LowPart = kernelTime.dwLowDateTime;
        kernel.HighPart = kernelTime.dwHighDateTime;
        user.LowPart = userTime.dwLowDateTime;
        user.HighPart = userTime.dwHighDateTime;
        
        // 如果是第一次调用，只记录时间，不计算使用率
        if (lastIdleTime_.dwLowDateTime == 0 && lastIdleTime_.dwHighDateTime == 0) {
            lastIdleTime_ = idleTime;
            lastKernelTime_ = kernelTime;
            lastUserTime_ = userTime;
            return 0.0;
        }
        
        ULARGE_INTEGER lastIdle, lastKernel, lastUser;
        lastIdle.LowPart = lastIdleTime_.dwLowDateTime;
        lastIdle.HighPart = lastIdleTime_.dwHighDateTime;
        lastKernel.LowPart = lastKernelTime_.dwLowDateTime;
        lastKernel.HighPart = lastKernelTime_.dwHighDateTime;
        lastUser.LowPart = lastUserTime_.dwLowDateTime;
        lastUser.HighPart = lastUserTime_.dwHighDateTime;
        
        qint64 idleDiff = idle.QuadPart - lastIdle.QuadPart;
        qint64 kernelDiff = kernel.QuadPart - lastKernel.QuadPart;
        qint64 userDiff = user.QuadPart - lastUser.QuadPart;
        
        qint64 totalDiff = kernelDiff + userDiff;
        
        if (totalDiff > 0) {
            double cpuUsage = (double)(totalDiff - idleDiff) / totalDiff * 100.0;
            
            lastIdleTime_ = idleTime;
            lastKernelTime_ = kernelTime;
            lastUserTime_ = userTime;
            
            return qBound(0.0, cpuUsage, 100.0);
        }
    }
    
    return 0.0;
}

double SystemMonitor::calculateMemoryUsage()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    
    if (GlobalMemoryStatusEx(&memInfo)) {
        double usedMemory = (double)(memInfo.ullTotalPhys - memInfo.ullAvailPhys);
        double totalMemory = (double)memInfo.ullTotalPhys;
        
        if (totalMemory > 0) {
            return (usedMemory / totalMemory) * 100.0;
        }
    }
    
    return 0.0;
}

void SystemMonitor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        QString output = process_->readAllStandardOutput();
        QRegularExpression re("(\\d+)");
        QRegularExpressionMatch match = re.match(output);

        if (match.hasMatch()) {
            bool ok;
            int usage = match.captured(1).toInt(&ok);
            if (ok && usage != lastGpuUsage_) {
                lastGpuUsage_ = usage;
                emit gpuInfoUpdated(usage);
            }
        }
    }
}

void SystemMonitor::calculateGpuUsage()
{
    process_->start("nvidia-smi", { "--query-gpu=utilization.gpu", "--format=csv,noheader,nounits" });
}
