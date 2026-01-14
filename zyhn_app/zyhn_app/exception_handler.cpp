#include "exception_handler.h"
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <Windows.h>
#include <DbgHelp.h>
#include <QDebug>
#pragma comment(lib, "DbgHelp.lib")

QString ExceptionHandler::dumpPath_;

static long __stdcall ExceptionFilterWrapper(EXCEPTION_POINTERS* exceptionInfo)
{
    return ExceptionHandler::unhandledExceptionFilter(exceptionInfo);
}

long ExceptionHandler::unhandledExceptionFilter(EXCEPTION_POINTERS* exceptionInfo)
{
    QString dumpFilePath = getDumpPath();
    
    QString message = QString("应用程序发生未处理的异常，程序即将退出。\n\n")
                     + QString("崩溃文件已保存到：\n%1\n\n")
                       .arg(dumpFilePath)
                     + QString("请联系技术支持并提供此文件。");
    
    ::MessageBoxW(nullptr, 
                  (const wchar_t*)message.utf16(), 
                  L"应用程序异常", 
                  MB_OK | MB_ICONERROR);
    
    generateDump(exceptionInfo, dumpFilePath);    
    return EXCEPTION_EXECUTE_HANDLER;
}

void ExceptionHandler::initialize()
{
    if (dumpPath_.isEmpty()) {
        QString appPath = QDir::currentPath();
        dumpPath_ = appPath + "/dumps";
        QDir().mkpath(dumpPath_);
    }
    SetUnhandledExceptionFilter(ExceptionFilterWrapper);    
}

QString ExceptionHandler::getDumpPath()
{
    if (dumpPath_.isEmpty()) {
        QString appPath = QDir::currentPath();
        dumpPath_ = appPath + "/dumps";
        QDir().mkpath(dumpPath_);
    }
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz");
    QString fileName = QString("crash_%1.dmp").arg(timestamp);
    
    return dumpPath_ + "/" + fileName;
}

void ExceptionHandler::setDumpPath(const QString& path)
{
    dumpPath_ = path;
    QDir().mkpath(dumpPath_);
}

bool ExceptionHandler::generateDump(EXCEPTION_POINTERS* exceptionInfo, const QString& dumpPath)
{
    HANDLE hFile = CreateFileW(
        (const wchar_t*)dumpPath.utf16(),
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
    
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    MINIDUMP_EXCEPTION_INFORMATION exceptionParam;
    exceptionParam.ThreadId = GetCurrentThreadId();
    exceptionParam.ExceptionPointers = exceptionInfo;
    exceptionParam.ClientPointers = FALSE;
    
    MINIDUMP_TYPE dumpType = static_cast<MINIDUMP_TYPE>(
        MiniDumpWithDataSegs |
        MiniDumpWithHandleData |
        MiniDumpWithIndirectlyReferencedMemory |
        MiniDumpScanMemory);
    
    bool result = MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        dumpType,
        exceptionInfo ? &exceptionParam : nullptr,
        nullptr,
        nullptr
    );
    
    CloseHandle(hFile);
    
    return result != FALSE;
}

