#include "Log_text_browser.h"
#include "log4qt/logger.h"
#include "log4qt/patternlayout.h"
#include "text_browser_appender.h"
#include <Log4Qt/Level.h>
#include <Log4Qt/LoggerRepository.h>

LogTextBrowser::LogTextBrowser(QWidget* parent)
	: QTextBrowser(parent), m_cur_log_level(Log4Qt::Level::Value::ALL_INT)
{
    initColor();

    Log4Qt::Logger* logger = Log4Qt::Logger::rootLogger();

    Log4Qt::PatternLayout* layout = new Log4Qt::PatternLayout();
    layout->setConversionPattern("%d{yy-MM-dd HH:mm:ss.zzz} [%p]   %c - %m%n");
    layout->activateOptions();

    m_text_browser_appender = new TextBrowserAppender(this);
    m_text_browser_appender->setName("TextBrowserAppender");
    m_text_browser_appender->setLayout(layout);
    connect(m_text_browser_appender, &TextBrowserAppender::sendMessage,
        this, &LogTextBrowser::onRecvLogMsg,
        Qt::QueuedConnection);
    

    logger->addAppender(m_text_browser_appender);
    logger->setLevel(Log4Qt::Level::DEBUG_INT);
}

LogTextBrowser::~LogTextBrowser()
{
    Log4Qt::LoggerRepository* repository = Log4Qt::Logger::rootLogger()->loggerRepository();
    if (repository) {
        repository->shutdown();
    }
}

void LogTextBrowser::initColor()
{
    m_map_color.clear();
    m_map_color.insert(Log4Qt::Level::DEBUG_INT, "white");  // 调试信息
    m_map_color.insert(Log4Qt::Level::INFO_INT, "#4EC9B0"); // 普通信息 
    m_map_color.insert(Log4Qt::Level::WARN_INT, "orange"); // 警告信息 
    m_map_color.insert(Log4Qt::Level::ERROR_INT, "red"); //  错误信息 
    m_map_color.insert(Log4Qt::Level::FATAL_INT, "darkred"); // 严重错误信息 
    m_map_color.insert(Log4Qt::Level::NULL_INT, "#4EC9B0"); // 其他 
}

QString LogTextBrowser::formatLog(const QString& color, const QString& logStr) {
    return QString("<div style=\"color: %1; margin: 2px 0;\">%2</div>")
        .arg(color)
        .arg(logStr.toHtmlEscaped());
}

QString LogTextBrowser::getLogKey(const int level)
{
    switch (level)
    {
    case Log4Qt::Level::DEBUG_INT:
        return "[DEBUG]";
    case Log4Qt::Level::INFO_INT:
        return "[INFO]";
    case Log4Qt::Level::WARN_INT:
        return "[WARN]";
    case Log4Qt::Level::ERROR_INT:
        return "[ERROR]";
    case Log4Qt::Level::FATAL_INT:
        return "[FATAL]";
    default:
        return "";
    }
}

int LogTextBrowser::getLogLevel(const QString& log)
{
    if (log.contains("[DEBUG]")) {
        return Log4Qt::Level::DEBUG_INT;
    }
    else if (log.contains("[INFO]")) {
        return Log4Qt::Level::INFO_INT;
    }
    else if (log.contains("[WARN]")) {
        return Log4Qt::Level::WARN_INT;
    }
    else if (log.contains("[ERROR]")) {
        return Log4Qt::Level::ERROR_INT;
    }
    else if (log.contains("[FATAL]")) {
        return Log4Qt::Level::FATAL_INT;
    }
    return Log4Qt::Level::ALL_INT;
}

void LogTextBrowser::onRecvLogMsg(int level, const QString& logStr)
{
    m_all_logs.push_back(logStr);

    if (m_cur_log_level != Log4Qt::Level::Value::ALL_INT && level != m_cur_log_level) {
        return;
	}
    
    append(formatLog(m_map_color.value(level, "#4EC9B0"), logStr));
}

void LogTextBrowser::showLogByLevel(const int level)
{
    if (level == m_cur_log_level) {
        return;
    }

	m_cur_log_level = level;
    clear();

    QString log_key = getLogKey(m_cur_log_level);
    for (const QString& log : m_all_logs) {
        if (log.contains(log_key)) {
            append(formatLog(m_map_color.value(getLogLevel(log), "#4EC9B0"), log));
        }
	}
}

void LogTextBrowser::filterLog(const QString& key)
{
	clear();

    for (const QString& log : m_all_logs) {
        if (log.contains(key)) {
            append(formatLog(m_map_color.value(getLogLevel(log), "#4EC9B0"), log));
        }
    }
}

void LogTextBrowser::clearAllLog()
{
    m_all_logs.clear();
    clear();
}