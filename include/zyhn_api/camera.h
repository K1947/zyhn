#ifndef CAMERA_H
#define CAMERA_H

#include <QStringList>
#include <QMap>
#include <QObject>
#include <QImage>
#include <QHash>
#include <QDebug>
#include <stdint.h>
#include "zyhn_api_global.h"

namespace zyhn_core {
    // 前向声明
    class Camera;

    /*!
     * \brief The FrameInfo struct   帧率信息结构体
     *        存储相机帧率相关参数
     */
    struct FrameInfo {
        float minFrame;     //!< 最小帧率
        float maxFrame;     //!< 最大帧率
        float frameVal;     //!< 当前帧率
    };

    /*!
    * \brief The DeviceType enum    设备类型枚举
    *        USB: USB接口相机
    *        GIGE: 千兆网接口相机
    */
    enum DeviceType {
        VIRTUAL_DEVICE,
        USB,
        GIGE,
        CAMERALINK,
        DT_FINAL,
    };

    const QString DeviceTypeDescs[] = {
        QStringLiteral("Virtual"),
        QStringLiteral("Usb"),
        QStringLiteral("Gige"),
        QStringLiteral("Camera link")
    };

    /*!
     * \brief The CameraDriver class    抽象相机驱动类
     *        主要负责枚举对应品牌相机，实例化设备（针对具体某一品牌，如Basler相机）
     */
    class ZYHN_API_EXPORT CameraDriver : public QObject
    {
        Q_OBJECT
    public:
        virtual void getDeviceList() = 0;

        /*!
         * \brief 获取该驱动支持的所有相机名称列表
         * \return 相机名称列表
         * \note 纯虚函数，子类必须实现
         */
        virtual QStringList cameraNameList()const = 0;

        /*!
         * \brief 创建指定名称的相机实例
         * \param name 相机名称
         * \return 相机实例指针，创建失败返回nullptr
         * \note 纯虚函数，子类必须实现具体的相机实例化逻辑
         */
        virtual Camera* cameraInstance(const QString& name) = 0;
    };

    /*!
     * \brief   The Camera class    抽象相机类
     *          提供通用相机API接口
     * \warning 禁止用户实例化，由CameraDriver类负责实例化
     */
    class ZYHN_API_EXPORT Camera : public QObject
    {
        Q_OBJECT
            Q_PROPERTY(int gain READ gain WRITE setGain NOTIFY gainChanged)
            Q_PROPERTY(double exposure READ exposure WRITE setExposure NOTIFY exposureChanged)
            Q_PROPERTY(TriggerMode triggerMode READ triggerMode WRITE setTriggerMode NOTIFY triggerModeChanged)
    public:
        explicit Camera() : m_imgFormat(QImage::Format_Grayscale8) {}
        virtual ~Camera() {}
        /*!
         * \brief The TriggerMode enum    触发模式枚举
         *        Software: 软件触发模式
         *        Hardware: 硬件触发模式
         *        Continue: 连续采集模式
         */
        enum TriggerMode { Software = 0, Hardware = 1, Continue = 2 };

        /*!
         * \brief 判断是否为虚拟相机
         * \return 如果是虚拟相机返回true，否则返回false
         * \note 虚拟相机用于测试或模拟场景
         */
        virtual bool isVirtual()const { return false; }

        /*!
         * \brief 设置图像格式
         * \param format QImage格式枚举值
         */
        void setImageFormat(QImage::Format format)
        {
            m_imgFormat = format;
        }

        /*!
         * \brief 获取当前图像格式
         * \return QImage格式枚举值
         */
        QImage::Format imageFormat()const
        {
            return m_imgFormat;
        }

        QString name() {
            return camera_name_;
        }

        void setName(QString name) {
            camera_name_ = name;
        }

        /*!
         * \brief 设置设备ID
         * \param idName 设备ID名称
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类可重写实现具体功能
         */
        virtual bool setDeviceID(const QString idName)
        {
            Q_UNUSED(idName);
            return false;
        }

        /*!
         * \brief 获取设备ID
         * \param idName 输出参数，用于存储设备ID名称
         * \return 获取成功返回true，失败返回false
         * \note 默认实现返回false，子类可重写实现具体功能
         */
        virtual bool deviceID(QString& idName) const
        {
            Q_UNUSED(idName);
            return false;
        }

        /*!
         * \brief 设置采集模式
         * \param mode 采集模式字符串
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类可重写实现具体功能
         */
        virtual bool setAcquisitionMode(const QString mode)
        {
            Q_UNUSED(mode);
            return false;
        }

        /*!
         * \brief 获取当前采集模式
         * \return 采集模式字符串，如果未设置则返回空字符串
         * \note 默认实现返回空字符串，子类可重写实现具体功能
         */
        virtual QString acquisitionMode() const
        {
            return QString();
        }

        //**********************************相机操作*******************************/
            /*!
             * \brief 打开相机设备
             * \return 打开成功返回true，失败返回false
             * \note 纯虚函数，子类必须实现
             */
        virtual bool open() = 0;

        /*!
         * \brief 关闭相机设备
         * \note 纯虚函数，子类必须实现
         */
        virtual void close() = 0;

        /*!
         * \brief 检查相机是否已打开
         * \return 如果已打开返回true，否则返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool isOpened() const
        {
            return false;
        }

        /*!
         * \brief 设置相机事件
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类可重写实现具体功能
         */
        virtual bool setCameraEvent()
        {
            return false;
        }

        /*!
         * \brief 单次采图（非阻塞）
         * \return 采图启动成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool grabOne()
        {
            return false;
        }

        /*!
         * \brief 阻塞式单次采图
         * \param timeout 超时时间（毫秒）
         * \return 采图成功返回true，超时或失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool blockGrabOne(int timeout)
        {
            Q_UNUSED(timeout) return false;
        }

        /*!
         * \brief 开始连续采图
         * \return 启动成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool grabContinue()
        {
            return false;
        }

        /*!
         * \brief 停止采图
         * \return 停止成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool grabStop()
        {
            return false;
        }

        /*!
         * \brief 销毁设备资源
         * \note 默认实现为空，子类可重写以释放特定资源
         */
        virtual void destroyDevice() {}

        /*!
         * \brief 检查是否处于连续采图状态
         * \return 如果是连续采图状态返回true，否则返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool isContinue() const
        {
            return false;
        }

        //**********************************相机属性*******************************/
            /*!
             * \brief 获取当前增益值
             * \return 增益值，如果获取失败返回0
             * \note 默认实现返回0，子类应重写实现
             */
        virtual float gain()
        {
            return 0;
        }

        /*!
         * \brief 设置增益值
         * \param value 增益值，应在minGain()和maxGain()范围内
         * \note 默认实现为空，子类应重写实现
         */
        virtual void setGain(float value)
        {
            Q_UNUSED(value)
        }

        /*!
         * \brief 获取最大增益值
         * \return 最大增益值，如果获取失败返回0
         * \note 默认实现返回0，子类应重写实现
         */
        virtual float maxGain() const
        {
            return 0;
        }

        /*!
         * \brief 获取最小增益值
         * \return 最小增益值，如果获取失败返回0
         * \note 默认实现返回0，子类应重写实现
         */
        virtual float minGain() const
        {
            return 0;
        }

        /*!
         * \brief 获取当前曝光时间（微秒）
         * \return 曝光时间值，如果获取失败返回0
         * \note 默认实现返回0，子类应重写实现
         */
        virtual int exposure() const
        {
            return 0;
        }

        /*!
         * \brief 设置曝光时间（微秒）
         * \param value 曝光时间值，应在minExposure()和maxExposure()范围内
         * \note 默认实现为空，子类应重写实现
         */
        virtual void setExposure(int value)
        {
            Q_UNUSED(value)
        }

        /*!
         * \brief 获取最大曝光时间（微秒）
         * \return 最大曝光时间值，如果获取失败返回0
         * \note 默认实现返回0，子类应重写实现
         */
        virtual int maxExposure() const
        {
            return 0;
        }

        /*!
         * \brief 获取最小曝光时间（微秒）
         * \return 最小曝光时间值，如果获取失败返回0
         * \note 默认实现返回0，子类应重写实现
         */
        virtual int minExposure() const
        {
            return 0;
        }

        /*!
         * \brief 设置帧率
         * \param value 帧率值（帧/秒）
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool setFrameRate(float value)
        {
            Q_UNUSED(value) return false;
        }

        /*!
         * \brief 获取帧率信息
         * \param frameInfo 输出参数，用于存储帧率信息（最小、最大、当前值）
         * \return 获取成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool frameRate(FrameInfo& frameInfo) const
        {
            Q_UNUSED(frameInfo);
            return false;
        }

        //**********************************数字IO设置*******************************//
            /*!
             * \brief 设置闪光灯（Strobe）状态
             * \param state 闪光灯状态，true表示开启，false表示关闭
             * \return 设置成功返回true，失败返回false
             * \note 默认实现返回false，子类应重写实现
             */
        virtual bool setStrobeState(const bool state)
        {
            Q_UNUSED(state);
            return false;
        }

        /*!
         * \brief 获取闪光灯（Strobe）状态
         * \return 当前闪光灯状态，true表示开启，false表示关闭
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool strobeState() const
        {
            return false;
        }

        /*!
         * \brief 设置IO线选择项（LineSelector）
         * \param selector IO线选择项名称
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool setIOLineSelector(const QString selector)
        {
            Q_UNUSED(selector);
            return false;
        }

        /*!
         * \brief 获取当前IO线选择项（LineSelector）
         * \return IO线选择项名称，如果未设置则返回空字符串
         * \note 默认实现返回空字符串，子类应重写实现
         */
        virtual QString IOLineSelector() const
        {
            return QString();
        }

        /*!
         * \brief 设置IO模式（LineMode）
         * \param mode IO模式名称，如"Input"或"Output"
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool setIOMode(const QString mode)
        {
            Q_UNUSED(mode);
            return false;
        }

        /*!
         * \brief 获取当前IO模式（LineMode）
         * \return IO模式名称，如果未设置则返回空字符串
         * \note 默认实现返回空字符串，子类应重写实现
         */
        virtual QString IOMode() const
        {
            return QString();
        }

        /*!
         * \brief 设置IO线源（LineSource）
         * \param source IO线源名称
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool setlineSource(const QString source)
        {
            Q_UNUSED(source);
            return false;
        }

        /*!
         * \brief 获取当前IO线源（LineSource）
         * \return IO线源名称，如果未设置则返回空字符串
         * \note 默认实现返回空字符串，子类应重写实现
         */
        virtual QString lineSource() const
        {
            return QString();
        }

        /*!
         * \brief 设置IO线持续时间（LineDuration）
         * \param duration 持续时间值（微秒）
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool setLineDurate(const int duration)
        {
            Q_UNUSED(duration);
            return false;
        }

        /*!
         * \brief 获取IO线持续时间（LineDuration）
         * \return 持续时间值（微秒），如果获取失败返回0
         * \note 默认实现返回0，子类应重写实现
         */
        virtual int lineDurate() const
        {
            return 0;
        }

        /*!
         * \brief 设置IO线延迟时间（LineDelay）
         * \param delay 延迟时间值（微秒）
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool setLineDelay(const int delay)
        {
            Q_UNUSED(delay);
            return false;
        }

        /*!
         * \brief 获取IO线延迟时间（LineDelay）
         * \return 延迟时间值（微秒），如果获取失败返回0
         * \note 默认实现返回0，子类应重写实现
         */
        virtual int lineDelay() const
        {
            return 0;
        }

        /*!
         * \brief 设置IO线前置持续时间（LinePreDuration）
         * \param preDuration 前置持续时间值（微秒）
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool setLinePreDurate(const int preDuration)
        {
            Q_UNUSED(preDuration);
            return false;
        }

        /*!
         * \brief 获取IO线前置持续时间（LinePreDuration）
         * \return 前置持续时间值（微秒），如果获取失败返回0
         * \note 默认实现返回0，子类应重写实现
         */
        virtual int linePreDurate() const
        {
            return 0;
        }

        //************************************参数加载保存**************************/
            /*!
             * \brief 从文件加载相机参数
             * \param filePath 参数文件路径
             * \return 加载成功返回true，失败返回false
             * \note 默认实现返回false，子类应重写实现
             */
        virtual bool loadPam(const QString filePath)
        {
            Q_UNUSED(filePath);
            return false;
        }

        /*!
         * \brief 保存相机参数到文件
         * \param filePath 参数文件保存路径
         * \return 保存成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool savePam(const QString filePath)
        {
            Q_UNUSED(filePath);
            return false;
        }

        //************************************触发模式设置**************************/
            /*!
             * \brief 获取触发模式
             * \return 当前触发模式（Software/Hardware/Continue）
             * \note 默认实现返回Software，子类应重写实现
             */
        virtual TriggerMode triggerMode() const
        {
            return Software;
        }

        /*!
         * \brief 设置触发模式
         * \param mode 触发模式，默认为Software（软件触发）
         * \note 默认实现为空，子类应重写实现
         */
        virtual void setTriggerMode(TriggerMode mode = Software)
        {
            Q_UNUSED(mode)
        }

        /*!
         * \brief 设置触发状态
         * \param state 触发状态，true表示使能触发，false表示禁用触发
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool setTriggerState(const bool state)
        {
            Q_UNUSED(state);
            return false;
        }

        /*!
         * \brief 获取触发状态
         * \return 当前触发状态，true表示已使能，false表示未使能
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool triggerState() const
        {
            return false;
        }

        /*!
         * \brief 设置触发选择项（TriggerSelector）
         * \param selector 触发选择项名称，如"FrameStart"
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool setTriggerSelector(const QString selector)
        {
            Q_UNUSED(selector);
            return false;
        }

        /*!
         * \brief 获取当前触发选择项（TriggerSelector）
         * \return 触发选择项名称，如果未设置则返回空字符串
         * \note 默认实现返回空字符串，子类应重写实现
         */
        virtual QString triggerSelector() const
        {
            return QString();
        }

        /*!
         * \brief 设置触发激活方式（TriggerActive）
         * \param active 触发激活方式，如"RisingEdge"（上升沿）、"FallingEdge"（下降沿）
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool setTriggerActive(const QString active)
        {
            Q_UNUSED(active);
            return false;
        }

        /*!
         * \brief 获取当前触发激活方式（TriggerActive）
         * \return 触发激活方式名称，如果未设置则返回空字符串
         * \note 默认实现返回空字符串，子类应重写实现
         */
        virtual QString triggerActive() const
        {
            return QString();
        }

        /*!
         * \brief 设置触发延迟时间
         * \param delay 延迟时间值（微秒）
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool setTrigDelay(const float delay)
        {
            Q_UNUSED(delay);
            return false;
        }

        /*!
         * \brief 获取触发延迟时间
         * \return 延迟时间值（微秒），如果获取失败返回0
         * \note 默认实现返回0，子类应重写实现
         */
        virtual float trigDelay() const
        {
            return 0;
        }

        /*!
         * \brief 设置触发源（TriggerSource）
         * \param source 触发源名称，如"Line0"、"Line1"等
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool setTriggerSource(const QString source)
        {
            Q_UNUSED(source);
            return false;
        }

        /*!
         * \brief 获取当前触发源（TriggerSource）
         * \return 触发源名称，如果未设置则返回空字符串
         * \note 默认实现返回空字符串，子类应重写实现
         */
        virtual QString triggerSource() const
        {
            return QString();
        }

        //**********************************网口设置*******************************/
            /*!
             * \brief 设置IP配置（IP地址、子网掩码、默认网关）
             * \param ipAddress IP地址列表，格式为[ip1, ip2, ip3, ip4]
             * \param subnetMask 子网掩码列表，格式为[mask1, mask2, mask3, mask4]
             * \param defaultGateway 默认网关列表，格式为[gw1, gw2, gw3, gw4]
             * \return 设置成功返回true，失败返回false
             * \note 默认实现返回false，子类应重写实现，仅适用于网口相机
             */
        virtual bool setIPConfig(const QList<int>& ipAddress,
            const QList<int>& subnetMask,
            const QList<int>& defaultGateway)
        {
            Q_UNUSED(ipAddress);
            Q_UNUSED(subnetMask);
            Q_UNUSED(defaultGateway);
            return false;
        }

        /*!
         * \brief 获取IP地址
         * \return IP地址列表，格式为[ip1, ip2, ip3, ip4]，如果获取失败返回[0,0,0,0]
         * \note 默认实现返回[0,0,0,0]，子类应重写实现，仅适用于网口相机
         */
        virtual QList<int> getIPAddress()
        {
            return QList<int> {0, 0, 0, 0};
        }

        /*!
         * \brief 设置IP地址
         * \param ipAddress IP地址列表，格式为[ip1, ip2, ip3, ip4]
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现，仅适用于网口相机
         */
        virtual bool setIPAddress(const QList<int>& ipAddress)
        {
            Q_UNUSED(ipAddress);
            return false;
        }

        /*!
         * \brief 获取子网掩码
         * \return 子网掩码列表，格式为[mask1, mask2, mask3, mask4]，如果获取失败返回[0,0,0,0]
         * \note 默认实现返回[0,0,0,0]，子类应重写实现，仅适用于网口相机
         */
        virtual QList<int> getSubnetMask()
        {
            return QList<int> {0, 0, 0, 0};
        }

        /*!
         * \brief 设置子网掩码
         * \param subnetMask 子网掩码列表，格式为[mask1, mask2, mask3, mask4]
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现，仅适用于网口相机
         */
        virtual bool setSubnetMask(const QList<int>& subnetMask)
        {
            Q_UNUSED(subnetMask);
            return false;
        }

        /*!
         * \brief 获取默认网关
         * \return 默认网关列表，格式为[gw1, gw2, gw3, gw4]，如果获取失败返回[0,0,0,0]
         * \note 默认实现返回[0,0,0,0]，子类应重写实现，仅适用于网口相机
         */
        virtual QList<int> getDefaultGateway()
        {
            return QList<int> {0, 0, 0, 0};
        }

        /*!
         * \brief 设置默认网关
         * \param defaultGateway 默认网关列表，格式为[gw1, gw2, gw3, gw4]
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现，仅适用于网口相机
         */
        virtual bool setDefaultGateway(const QList<int>& defaultGateway)
        {
            Q_UNUSED(defaultGateway);
            return false;
        }

        //************************************图像格式设置**************************/
            /*!
             * \brief 设置像素格式
             * \param format 像素格式名称，如"Mono8"、"RGB24"等
             * \return 设置成功返回true，失败返回false
             * \note 默认实现返回false，子类应重写实现
             */
        virtual bool setPixelFormat(const QString format)
        {
            Q_UNUSED(format);
            return false;
        }

        /*!
         * \brief 获取当前像素格式
         * \return 像素格式名称，如果未设置则返回空字符串
         * \note 默认实现返回空字符串，子类应重写实现
         */
        virtual QString pixelFormat() const
        {
            return QString();
        }

        /*!
         * \brief 设置像素大小
         * \param size 像素大小字符串
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool setPixelSize(const QString size)
        {
            Q_UNUSED(size);
            return false;
        }

        /*!
         * \brief 获取当前像素大小
         * \return 像素大小字符串，如果未设置则返回空字符串
         * \note 默认实现返回空字符串，子类应重写实现
         */
        virtual QString pixelSize() const
        {
            return QString();
        }

        /*!
         * \brief 设置X方向镜像
         * \param reverse true表示镜像，false表示不镜像
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool reverseX(const bool reverse)
        {
            Q_UNUSED(reverse);
            return false;
        }

        /*!
         * \brief 设置Y方向镜像
         * \param reverse true表示镜像，false表示不镜像
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool reverseY(const bool reverse)
        {
            Q_UNUSED(reverse);
            return false;
        }

        /*!
         * \brief 检查X方向是否镜像
         * \return 如果已镜像返回true，否则返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool isReverseX() const
        {
            return false;
        }

        /*!
         * \brief 检查Y方向是否镜像
         * \return 如果已镜像返回true，否则返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool isReverseY() const
        {
            return false;
        }

        /*!
         * \brief 设置图像节点数量
         * \param num 图像节点数量
         * \return 设置成功返回true，失败返回false
         * \note 默认实现返回false，子类应重写实现
         */
        virtual bool setImageNodeNum(const int num)
        {
            Q_UNUSED(num);
            return false;
        }

        /*!
         * \brief 获取图像节点数量
         * \return 图像节点数量，默认返回1
         * \note 默认实现返回1，子类应重写实现
         */
        virtual int imageNodeNum() const
        {
            return 1;
        }

        //************************************相机状态设置**************************/
            /*!
             * \brief 设置相机就绪状态
             * \param value 就绪状态，true表示就绪，false表示未就绪
             * \note 设置后会触发ready()信号
             */
        void setReady(bool value = true)
        {
            m_ready = value;
            emit ready();
        }

        /*!
         * \brief 清除相机状态
         *        将就绪状态和采图错误状态都设置为false
         */
        void clear()
        {
            m_ready = false;
            m_grabErr = false;
        }

        /*!
         * \brief 检查相机是否就绪
         * \return 如果相机就绪返回true，否则返回false
         */
        bool isReady() const
        {
            return m_ready;
        }

        /*!
         * \brief 检查是否有采图错误
         * \return 如果有错误返回true，否则返回false
         */
        bool isGrabErr() const
        {
            return m_grabErr;
        }

        /*!
         * \brief 获取当前图像
         * \return 当前采集的图像，如果无图像则返回空图像
         * \note 纯虚函数，子类必须实现
         */
        virtual QImage image() const = 0;

        // 设备类型
        virtual DeviceType deviceType() const
        {
            return DeviceType::VIRTUAL_DEVICE;
        }

    signals:
        /*!
         * \brief 相机用户名称改变信号
         *        当相机的用户名称发生变化时触发
         */
        void camUserNameChange();

        /*!
         * \brief 相机打开信号
         *        当相机成功打开时触发
         */
        void opened();

        /*!
         * \brief 相机关闭信号
         *        当相机关闭时触发
         */
        void closed();

        /*!
         * \brief 相机就绪信号
         *        当相机就绪状态改变时触发
         */
        void ready();

        /*!
         * \brief 采图错误信号
         *        当采图过程中发生错误时触发
         */
        void grabErr();

        /*!
         * \brief 增益值改变信号
         *        当增益值发生变化时触发
         */
        void gainChanged();

        /*!
         * \brief 曝光时间改变信号
         *        当曝光时间发生变化时触发
         */
        void exposureChanged();

        /*!
         * \brief 触发模式改变信号
         *        当触发模式发生变化时触发
         */
        void triggerModeChanged();

        /*!
         * \brief 帧率改变信号
         *        当帧率设置发生变化时触发
         */
        void frameRateChanged();

        /*!
         * \brief 连续采集状态改变信号
         *        当连续采集状态发生变化时触发
         */
        void contiuneChanged();

        void imageGrabed(const QImage&);
    protected:
        friend class CameraFactory;                    //!< 允许CameraFactory访问私有成员

        QString camera_name_;                                //!< 相机的唯一标识ID
        static QHash<QString, Camera*> camera_map_;      //!< 静态ID到相机实例的映射表
        QImage::Format m_imgFormat;                   //!< 图像格式

        bool m_ready;      //!< 采图就绪状态标志
        bool m_grabErr;    //!< 采图错误状态标志
    };
}

#endif // !CAMERA_H

