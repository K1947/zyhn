#include "about_dlg.h"
#include "utils.h"
#include <windows.h>
#include <QHostInfo>
#include <QOperatingSystemVersion>
#include <QDir>
#include <QNetworkInterface>
#include <IPHlpApi.h>
#include <versionhelpers.h>
#pragma comment(lib, "version.lib")
#pragma comment(lib, "Iphlpapi.lib")

AboutDlg::AboutDlg(QWidget* parent) :BaseDlg(parent)
{
    ui_.setupUi(ContentArea());
    setTitleText("关于我们");
    zyhn_core::Utils::setStyleSheet(this->ContentArea(), ":/zyhn_app/style/qss/about_dlg.qss");
    
    resize(500, 300);
    setResizable(false);

    ui_.label_pc_name->setText(QHostInfo::localHostName());
    ui_.label_system->setText("WINDOWS NT " + QSysInfo::kernelVersion());
    ui_.label_architecture->setText(getWindowsArchitecture());
    ui_.label_sysytem_ver->setText(getWindowsOSVersion());
    ui_.label_macaddress->setText(getMacAddress());
    ui_.label_ip->setText(getIpAddress());

    ui_.label_version->setText(fileVersion(qApp->applicationFilePath()));
}

AboutDlg::~AboutDlg()
{

}

QString AboutDlg::getWindowsArchitecture()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    switch (si.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64:
        return "x64";
    case PROCESSOR_ARCHITECTURE_ARM:
        return "ARM";
    case PROCESSOR_ARCHITECTURE_ARM64:
        return "ARM64";
    case PROCESSOR_ARCHITECTURE_IA64:
        return "IA64";
    case PROCESSOR_ARCHITECTURE_INTEL:
        return "x86";
    default:
        return "Unknown";
    }
}

QString AboutDlg::getWindowsOSVersion()
{
    QOperatingSystemVersion version = QOperatingSystemVersion::current();
    QString name = version.name();
    QString versionStr = QString("%1.%2").arg(version.majorVersion()).arg(version.minorVersion());

    if (name == "Windows") {
        if (version.majorVersion() == 10 && version.microVersion() >= 22000) {
            return "Windows 11";
        } else if (versionStr == "10.0") {
            return "Windows 10";
        }
        else if (versionStr == "6.3") {
            return "Windows 8.1";
        }
        else if (versionStr == "6.2") {
            return "Windows 8";
        }
        else if (versionStr == "6.1") {
            return "Windows 7";
        }
    }

    return name + " " + versionStr;
}

QString AboutDlg::fileVersion(const QString& filePath)
{
    DWORD dummy;
    QString path = QDir::toNativeSeparators(filePath);
    DWORD size = GetFileVersionInfoSizeW(reinterpret_cast<LPCWSTR>(path.utf16()), &dummy);
    if (!size) return {};

    QByteArray block(size, 0);
    if (!GetFileVersionInfoW(reinterpret_cast<LPCWSTR>(path.utf16()),
        0, size, reinterpret_cast<LPVOID>(block.data())))
        return {};

    VS_FIXEDFILEINFO* info = nullptr;
    UINT len = 0;
    if (!VerQueryValueW(block.data(), L"\\", reinterpret_cast<LPVOID*>(&info), &len))
        return {};
    if (!info || len < sizeof(VS_FIXEDFILEINFO)) return {};

    return QString("软件版本：%1.%2.%3.%4")
        .arg(HIWORD(info->dwFileVersionMS))
        .arg(LOWORD(info->dwFileVersionMS))
        .arg(HIWORD(info->dwFileVersionLS))
        .arg(LOWORD(info->dwFileVersionLS));
}

QString AboutDlg::getMacAddress()
{
    QStringList macAddresses;
    
    // 方法1: 使用 Qt 的 QNetworkInterface
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface& ni : interfaces) {
        if (ni.flags().testFlag(QNetworkInterface::IsUp) &&
            ni.flags().testFlag(QNetworkInterface::IsRunning) &&
            !ni.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            QString mac = ni.hardwareAddress();
            if (!mac.isEmpty() && mac != "00:00:00:00:00:00") {
                macAddresses.append(mac);
            }
        }
    }
    
    // 如果没有找到，使用 Windows API
    if (macAddresses.isEmpty()) {
        PIP_ADAPTER_INFO adapterInfo = nullptr;
        ULONG bufferSize = sizeof(IP_ADAPTER_INFO);
        
        adapterInfo = (IP_ADAPTER_INFO*)malloc(bufferSize);
        if (GetAdaptersInfo(adapterInfo, &bufferSize) == ERROR_BUFFER_OVERFLOW) {
            free(adapterInfo);
            adapterInfo = (IP_ADAPTER_INFO*)malloc(bufferSize);
        }
        
        if (GetAdaptersInfo(adapterInfo, &bufferSize) == NO_ERROR) {
            PIP_ADAPTER_INFO currentAdapter = adapterInfo;
            while (currentAdapter) {
                if (currentAdapter->Type == MIB_IF_TYPE_ETHERNET || 
                    currentAdapter->Type == IF_TYPE_IEEE80211) {
                    QString mac;
                    for (int i = 0; i < currentAdapter->AddressLength; ++i) {
                        if (i > 0) mac += ":";
                        mac += QString::number(currentAdapter->Address[i], 16).rightJustified(2, '0');
                    }
                    if (!mac.isEmpty()) {
                        macAddresses.append(mac.toUpper());
                    }
                }
                currentAdapter = currentAdapter->Next;
            }
        }
        
        if (adapterInfo) {
            free(adapterInfo);
        }
    }
    
    return macAddresses.isEmpty() ? "未找到" : macAddresses.join(", ");
}

QString AboutDlg::getIpAddress()
{
    QStringList ipAddresses;
    
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface& ni : interfaces) {
        if (ni.flags().testFlag(QNetworkInterface::IsUp) &&
            ni.flags().testFlag(QNetworkInterface::IsRunning) &&
            !ni.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            
            QList<QNetworkAddressEntry> entries = ni.addressEntries();
            for (const QNetworkAddressEntry& entry : entries) {
                QHostAddress address = entry.ip();
                if (address.protocol() == QAbstractSocket::IPv4Protocol) {
                    QString ip = address.toString();
                    if (!ipAddresses.contains(ip)) {
                        ipAddresses.append(ip);
                    }
                }
            }
        }
    }
    
    return ipAddresses.isEmpty() ? "127.0.0.1" : ipAddresses.join(", ");
}