#ifndef ALGO_CONTENT_VIEW_H
#define ALGO_CONTENT_VIEW_H

#include <QWidget>
#include <QTextBrowser>
#include "configurable_object.h"
#include "image_editor.h"
#include "property_editor_widget.h"
#include "vision_core_global.h"
#include "log_browser.h"

class VISION_CORE_EXPORT AlgoContentView: public QWidget
{
    Q_OBJECT
public:
    AlgoContentView(QWidget* propEditor, QWidget* parent = nullptr);
    ~AlgoContentView();
public:
    ImageEditor* imageEditor() const;
    PropertyEditorWidget* propEditor() const;
    void debug(const QString& message, const QString& category = QString());
    void info(const QString& message, const QString& category = QString());
    void warning(const QString& message, const QString& category = QString());
    void error(const QString& message, const QString& category = QString());
    void critical(const QString& message, const QString& category = QString());
private:
    ImageEditor* image_editor_;
    PropertyEditorWidget* prop_editor_;
    LogBrowser* log_browser_;
};

#endif // !ALGO_CONTENT_VIEW_H