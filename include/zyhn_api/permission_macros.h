#ifndef PERMISSION_MACROS_H
#define PERMISSION_MACROS_H

#include "permission_utils.h"
#include "toast.h"
#include <QWidget>

namespace zyhn_core {

    /**
     * @brief 权限检查宏定义 - 提供最简洁的权限检查方式
     */

     // 基础权限检查宏
#define CHECK_PERMISSION(permission) \
    zyhn_core::PermissionUtils::hasPermission(permission)

#define CHECK_AND_RETURN(permission, returnValue) \
    if (!CHECK_PERMISSION(permission)) { \
        qDebug() << "权限不足，需要权限:" << static_cast<int>(permission); \
        return returnValue; \
    }

#define CHECK_AND_RETURN_VOID(permission) \
    if (!CHECK_PERMISSION(permission)) { \
        qDebug() << "权限不足，需要权限:" << static_cast<int>(permission); \
        return; \
    }

// 带消息框的权限检查宏
#define CHECK_PERMISSION_WITH_MSG(permission, message) \
    if (!CHECK_PERMISSION(permission)) { \
        Toast::warning(message); \
        return false; \
    }

#define CHECK_PERMISSION_WITH_MSG_VOID(permission, message) \
    if (!CHECK_PERMISSION(permission)) { \
        Toast::warning(message); \
        return; \
    }

#define CHECK_PERMISSION_COMMON_MSG(permission) \
     if (!CHECK_PERMISSION(permission)) {\
        Toast::warning("权限不足"); \
        return; \
     }

// 权限检查并执行宏
#define IF_HAS_PERMISSION(permission, code) \
    if (CHECK_PERMISSION(permission)) { \
        code \
    }

#define IF_HAS_PERMISSION_ELSE(permission, ifCode, elseCode) \
    if (CHECK_PERMISSION(permission)) { \
        ifCode \
    } else { \
        elseCode \
    }

// 便捷的权限检查宏
#define CAN_MANAGE_USERS() CHECK_PERMISSION(P_UserManagement)
#define CAN_MANAGE_SYSTEM() CHECK_PERMISSION(P_SystemManagement)
#define CAN_VIEW_PRODUCTS() CHECK_PERMISSION(P_ProductView)
#define CAN_CREATE_PRODUCTS() CHECK_PERMISSION(P_ProductCreate)
#define CAN_EDIT_PRODUCTS() CHECK_PERMISSION(P_ProductEdit)
#define CAN_DELETE_PRODUCTS() CHECK_PERMISSION(P_ProductDelete)
#define CAN_VIEW_DEVICES() CHECK_PERMISSION(P_DeviceView)
#define CAN_CONTROL_DEVICES() CHECK_PERMISSION(P_DeviceControl)
#define CAN_CONFIG_DEVICES() CHECK_PERMISSION(P_DeviceConfig)
#define CAN_VIEW_VISION() CHECK_PERMISSION(P_VisionView)
#define CAN_RUN_VISION() CHECK_PERMISSION(P_VisionRun)
#define CAN_CONFIG_VISION() CHECK_PERMISSION(P_VisionConfig)
#define CAN_VIEW_DATA() CHECK_PERMISSION(P_DataView)
#define CAN_EXPORT_DATA() CHECK_PERMISSION(P_DataExport)
#define CAN_DELETE_DATA() CHECK_PERMISSION(P_DataDelete)

// 带返回值的权限检查宏
#define REQUIRE_PERMISSION(permission) \
    CHECK_AND_RETURN(permission, false)

#define REQUIRE_PERMISSION_VOID(permission) \
    CHECK_AND_RETURN_VOID(permission)

// 带消息框的权限检查宏
#define REQUIRE_PERMISSION_MSG(permission, message) \
    CHECK_PERMISSION_WITH_MSG(permission, message)

#define REQUIRE_PERMISSION_MSG_VOID(permission, message) \
    CHECK_PERMISSION_WITH_MSG_VOID(permission, message)

} // namespace zyhn_core

#endif // PERMISSION_MACROS_H
