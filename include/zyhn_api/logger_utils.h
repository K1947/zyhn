#ifndef LOGGER_UTILS_H
#define LOGGER_UTILS_H

#include <QString>
#include <QDebug>
#include "zyhn_api_global.h"

namespace Log4Qt {
    class Logger;
}

namespace zyhn_core {

    /**
     * @brief 简洁的日志工具类 
     */
    class ZYHN_API_EXPORT LoggerUtils
    {
    public:
        static Log4Qt::Logger* getLogger(const QString& module);
        static void debug(const QString& module, const QString& message);
        static void info(const QString& module, const QString& message);
        static void warn(const QString& module, const QString& message);
        static void error(const QString& module, const QString& message);
        static void fatal(const QString& module, const QString& message);
        template<typename... Args>
        static QString format(const QString& format, Args&&... args);
    };
} // namespace zyhn_core

#endif // LOGGER_UTILS_H
