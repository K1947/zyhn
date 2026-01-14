#include "text_browser_appender.h"
#include <Log4Qt/LoggingEvent.h>
#include <QTextBrowser>
#include <QDebug>
#include "log4qt/level.h"

TextBrowserAppender::TextBrowserAppender(QObject* parent)
    : Log4Qt::AppenderSkeleton(parent)
{}

TextBrowserAppender::~TextBrowserAppender() 
{
    close();
}

bool TextBrowserAppender::requiresLayout() const {
    return true;
}

void TextBrowserAppender::append(const Log4Qt::LoggingEvent& rEvent)
{
    // 屏蔽掉Qt的日志
    if (rEvent.loggename() == "Qt") {
        return;
    }

    QString formattedMessage = layout()->format(rEvent);

    emit sendMessage(rEvent.level().toInt(), formattedMessage);
}

void TextBrowserAppender::close()
{
    Log4Qt::AppenderSkeleton::close();
}
