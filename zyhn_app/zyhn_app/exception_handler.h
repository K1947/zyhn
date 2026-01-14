#ifndef EXCEPTION_HANDLER_H
#define EXCEPTION_HANDLER_H

#include <QString>
#include <Windows.h>

class ExceptionHandler
{
public:
    static void initialize();
    static QString getDumpPath();
    static void setDumpPath(const QString& path);
    static long unhandledExceptionFilter(EXCEPTION_POINTERS* exceptionInfo);
    static bool generateDump(EXCEPTION_POINTERS* exceptionInfo, const QString& dumpPath);
    static QString dumpPath_;
};

#endif // EXCEPTION_HANDLER_H

