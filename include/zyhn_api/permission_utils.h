#ifndef PERMISSION_UTILS_H
#define PERMISSION_UTILS_H

#include "zyhn_core.h"
#include <QApplication>
#include <QDebug>
#include <functional>
#include "zyhn_api_global.h"

namespace zyhn_core {

    /**
     * @brief 权限工具类
     */
    class ZYHN_API_EXPORT PermissionUtils
    {
    public:
        inline static IUserManager* userMgr_ = nullptr;
        static void setUserManager(IUserManager* userMgr);
        static IUserManager* getUserManager();
        static bool hasPermission(Permission permission);

        static bool isLoggedIn();

        static UserInfo getCurrentUser();

        static bool checkAndExecute(Permission permission,
            std::function<void()> action,
            std::function<void()> noPermissionAction = nullptr);


        template<typename T>
        static T checkAndReturn(Permission permission,
            std::function<T()> action,
            const T& defaultValue = T{});

        static bool canOpenVisionPanel();
        static bool canOpenControlPanel();
        static bool canOpenCommPanel();
        static bool canOpenMesPanel();
        static bool canViewProduct();
        static bool canViewDevice();
        static bool canViewDevices();
        static bool canViewOperate();
    };

} // namespace zyhn_core

#endif // PERMISSION_UTILS_H
