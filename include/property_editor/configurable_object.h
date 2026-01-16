#ifndef CONFIGURABLE_OBJECT_H
#define CONFIGURABLE_OBJECT_H

#include "propertyinfo.h"
#include <QObject>
#include <QVariantMap>
#include <QList>
#include <QJsonObject>
#include "property_editor_global.h"

class PROP_EDITOR_EXPORT ConfigurableObject : public QObject
{
    Q_OBJECT
public:
    explicit ConfigurableObject(QObject* parent = nullptr);

    void registerProperty(const PropertyInfo& prop);
    void registerBtn(int gropid, QString btnName);
    QVariant propValue(const QString& key) const;
    void setPropValue(const QString& key, const QVariant& value);
    const QList<PropertyInfo>& properties() const { return properties_; }
    QMap<int, QStringList> btnNames();
    QVariant enumData(QString key);
    int enumIndex(QString key) const;
    QVariant enumValue(const QString& key) const;
    bool hasProperty(const QString& key) const;
    PropertyInfo& propertyInfo(const QString& key) const;
    QList<QPair<QString, QVariant>> getEnumList(const QString& key) const;
    void addProperty(const PropertyInfo& prop);
    void removeProperty(const QString& key);
    void updateProperty(const PropertyInfo& prop);
    void setPropertyVisible(const QString& key, bool visible);
    void setPropertyReadOnly(const QString& key, bool readOnly);
    void setPropertyRange(const QString& key, const QVariant& min, const QVariant& max);
    void setPropertyEnumValues(const QString& key, const QList<QPair<QString, QVariant>>& enumValues);
    void setPropetyFromJson(const QJsonObject& obj);
    bool isRealUpdate() const;
signals:
    void propertyChanged(const QString& key, const QVariant& value);
    void btnClicked(const QString& btnName);
    void propertyAdded(const QString& key, const PropertyInfo& prop);
    void propertyRemoved(const QString& key);
    void propertyUpdated(const QString& key, const PropertyInfo& prop);
    void propertiesReset();

private:
    int findPropertyIndex(const QString& key) const;
private:
    QList<PropertyInfo> properties_;
    QVariantMap values_;
    QMap<int, QStringList> btn_names_;
};

#endif // CONFIGURABLEOBJECT_H