#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QString>
#include <QDebug>
#include <QProcess>

#include <windows.h>

class SystemMonitor : public QObject
{
    Q_OBJECT

public:
    explicit SystemMonitor(QObject *parent = nullptr);
    ~SystemMonitor();

    double getCpuUsage() const;
    double getMemoryUsage() const;
    int getGpuUsage() const;
    QString getCurrentTime() const;
    
    void startMonitoring(int intervalMs = 1000);
    void stopMonitoring();
    bool isMonitoring() const;
signals:
    void systemInfoUpdated(double cpuUsage, double memoryUsage, const QString& currentTime);
    void gpuInfoUpdated(int gpuUsage);

private slots:
    void updateSystemInfo();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    double calculateCpuUsage();
    double calculateMemoryUsage();
    void calculateGpuUsage();
    QString formatCurrentTime() const;

private:
    QTimer* timer_;
    bool isMonitoring_;    
    double lastCpuUsage_;
    qint64 lastCpuTime_;
    qint64 lastSystemTime_;
    double lastMemoryUsage_;
    int lastGpuUsage_;
    FILETIME lastIdleTime_;
    FILETIME lastKernelTime_;
    FILETIME lastUserTime_;
    QProcess* process_;
};

#endif // SYSTEM_MONITOR_H
