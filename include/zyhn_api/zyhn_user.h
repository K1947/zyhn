#ifndef ZYHN_USER_H
#define ZYHN_USER_H

#include <QObject>

namespace zyhn_core {

    // 用户角色枚举
    enum UserRole
    {
        UR_SuperAdmin = 0,    // 超级管理员
        UR_Admin = 1,         // 普通管理员
        UR_Operator = 2       // 一般操作员
    };

    // 权限枚举
    enum Permission
    {
        P_VisionPanel = 1ull << 1, // 视觉面板
        P_ControlPanel = 1ull << 2,   // 控制面板
        P_CommPanel = 1ull << 3,   // 通信面板
        P_MesPanel = 1ull << 4, // mes面板    
        P_ProductView = 1ull << 5,      // 查看产品
        P_DeviceView = 1ull << 6 ,     // 查看设备
        P_UserOperate = 1ull << 7,       // 用户操作
    };

    // 用户信息结构
    struct UserInfo
    {
        int id;
        QString username;
        QString realName;
        QString password;
        UserRole role;
        int permissions;   // 权限位掩码
        QString createTime;

        UserInfo() : id(-1), role(UR_Operator), permissions(0) {}
    };
}
#endif // !ZYHN_USER_H

