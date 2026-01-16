#ifndef TEXT_BROWSER_APPENDER_H
#define TEXT_BROWSER_APPENDER_H

#include <Log4Qt/AppenderSkeleton.h>

class TextBrowserAppender : public Log4Qt::AppenderSkeleton
{
    Q_OBJECT
public:
    explicit TextBrowserAppender(QObject* parent = nullptr);
    ~TextBrowserAppender();

signals:
    void sendMessage(int level, const QString& message);

protected:
    virtual void append(const Log4Qt::LoggingEvent& rEvent) override;
    virtual void close() override;
    virtual bool requiresLayout() const override;
};

#endif // !TEXT_BROWSER_APPENDER_H
