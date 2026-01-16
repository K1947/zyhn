#ifndef LOGBROWSER_H
#define LOGBROWSER_H

#include <QTextBrowser>
#include <QMenu>
#include <QContextMenuEvent>
#include <QTextCursor>
#include <QDateTime>
#include <QClipboard>
#include <QApplication>
#include "vision_core_global.h"

class VISION_CORE_EXPORT LogBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    enum LogLevel {
        LEVEL_DEBUG = 0,
        LEVEL_INFO,
        LEVEL_WARNING,
        LEVEL_ERROR,
        LEVEL_CRITICAL
    };

    Q_ENUM(LogLevel)

    explicit LogBrowser(QWidget* parent = nullptr);

    void logMessage(const QString& message, LogLevel level = LEVEL_INFO,
        const QString& category = QString());

    void debug(const QString& message, const QString& category = QString());
    void info(const QString& message, const QString& category = QString());
    void warning(const QString& message, const QString& category = QString());
    void error(const QString& message, const QString& category = QString());
    void critical(const QString& message, const QString& category = QString());

    void setShowTimestamp(bool show);
    void setShowLevel(bool show);
    void setShowCategory(bool show);
    void setShowSource(bool show);
    void setTimestampFormat(const QString& format);
    void setAutoScroll(bool autoScroll);
    void setMaxLineCount(int maxLines);
    void setLevelColor(LogLevel level, const QColor& color);

    QColor getLevelColor(LogLevel level) const;
    void clear();

signals:
    void logCleared();

protected:
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void copySelectedText();
    void selectAllText();
    void clearLog();

private:
    void setupContextMenu();
    QString getLevelString(LogLevel level) const;

    QMenu* context_menu_;
    QAction* copy_action_;
    QAction* select_all_action_;
    QAction* clear_action_;

    bool show_time_stamp_;
    bool show_level_;
    bool show_category_;
    bool show_source_;
    bool auto_scroll_;
    QString time_stamp_format_;
    int max_line_count_;

    QMap<LogLevel, QColor> level_colors_;
};

#endif // LOGBROWSER_H
