#ifndef PROPERTY_EDITOR_WIDGET_H
#define PROPERTY_EDITOR_WIDGET_H

#include <QTabWidget>
#include "propertyinfo.h"
#include <QMap>
#include "property_editor_global.h"
#include <QCheckBox>

class ConfigurableObject;
class QWidget;
class PROP_EDITOR_EXPORT PropertyEditorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PropertyEditorWidget(ConfigurableObject* obj, QWidget* parent = nullptr);
    void updateEditorValue(QString);
    void setPropValue(const QJsonObject& obj);
    QJsonObject getPropJson();
    ConfigurableObject* propObject() const { return config_object_; }
private slots:
    void onSaveClicked();
    void onLoadClicked();
    void onBtnClicked();
signals:
    void onBtnCliclkSignals(QString);
private:
    void buildTabs();
    QWidget* createEditorForProperty(const PropertyInfo& prop, const QVariant& value);
    void updateEditorFromObject();
    void setCommParams();

    ConfigurableObject* config_object_;
    QTabWidget* tab_widget_;
    QMap<QString, QWidget*> key_to_editor_;
    QMap<QWidget*, QString> widget_to_key_;
};

#endif // PROPERTYEDITORTABWIDGET_H