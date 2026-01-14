#ifndef LOG_TEXT_BROWSER_H 
#define LOG_TEXT_BROWSER_H

#include <QTextBrowser>

class TextBrowserAppender;
class LogTextBrowser: public QTextBrowser
{
	Q_OBJECT
public:
	LogTextBrowser(QWidget* parent = nullptr);
	~LogTextBrowser();
	void showLogByLevel(const int level);
	void filterLog(const QString& key);
	void clearAllLog();

private slots:
	void onRecvLogMsg(int, const QString&);

private:
	QString formatLog(const QString& color, const QString& log);
	void initColor();
	QString getLogKey(const int level);
	int     getLogLevel(const QString& log);

private:
	TextBrowserAppender* m_text_browser_appender;
	QStringList m_all_logs;  // 所有日志
	QMap<int, QString> m_map_color;
	QString m_filter_word;   // 过滤内容
	int m_cur_log_level; // 当前日志等级
};

#endif // !LOG_TEXT_BROWSER_H 
