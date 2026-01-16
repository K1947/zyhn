#ifndef PRODUCT_DEF_H
#define PRODUCT_DEF_H
#include <QObject>

namespace zyhn_product {

    enum class WorkProcedure {
        CHECK_THREAD = 0, // 检测流程
        POS_THREAD = 1, // 定位流程
        MARK_THREAD = 2, // 标定流程
    };
    Q_NAMESPACE
    Q_ENUM_NS(WorkProcedure)
    // ===== 通信方式 =====
    enum class CommType {
        SerialPort = 0,
        TCPClient = 1,
        PLC = 2,
    };
    Q_ENUM_NS(CommType)
    // ===== 触发方式 =====
    enum class TriggerType {
        PLC = 0,
        Command = 1,
    };
    Q_ENUM_NS(TriggerType)
    // ===== 保存图片类型 =====
    enum class SaveImageType {
        Origin = 0,
        Compress = 1,
    };
    Q_ENUM_NS(SaveImageType)
    
    // ===== 展示结果 =====
    enum class DisPlayResultPos {
       DPR_1 = 0,
       DPR_4 = 1,
       DPR_9 = 2,
       DPR_16 = 3,
       DPR_Custom =4
    };
    Q_ENUM_NS(DisPlayResultPos)
}
#endif // !PRODUCT_DEF_H

