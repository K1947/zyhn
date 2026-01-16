#ifndef PROPERTYINFO_H
#define PROPERTYINFO_H

#include <QString>
#include <QVariant>
#include <QMetaType>
#include "property_editor_global.h"

struct PropertyInfo {
    QString name;           // 显示名称
    QString key;            // 内部关键字
    QString category;       // 分类
    QMetaType::Type type;   // 类型
    QVariant defaultValue;
    QString toolTip;
    QVariant minValue;
    QVariant maxValue;
    QList<QPair<QString, QVariant>> enumValues;
};

class PROP_EDITOR_EXPORT PropertyBuilder {
public:
    static PropertyBuilder create(const QString& name, const QString& key) {
        return PropertyBuilder(name, key);
    }

    PropertyBuilder& category(const QString& cat) {
        info.category = cat;
        return *this;
    }

    PropertyBuilder& type(QMetaType::Type type) {
        info.type = type;
        return *this;
    }

    PropertyBuilder& defaultValue(const QVariant& value) {
        info.defaultValue = value;
        return *this;
    }

    PropertyBuilder& toolTip(const QString& tip) {
        info.toolTip = tip;
        return *this;
    }

    PropertyBuilder& range(const QVariant& min, const QVariant& max) {
        info.minValue = min;
        info.maxValue = max;
        return *this;
    }

    PropertyBuilder& addEnum(QString text, QVariant value) {
        info.enumValues.append(qMakePair(text, value));
        return *this;
    }

    PropertyBuilder& enums(std::initializer_list<QPair<QString, QVariant>> values) {
        for (const auto& [value, text] : values) {
            info.enumValues.append(qMakePair(value, text));
        }
        return *this;
    }

    PropertyInfo build() const {
        return info;
    }

    template<typename T>
    void registerTo(T* owner) {
        owner->registerProperty(info);
    }

private:
    PropertyBuilder(const QString& name, const QString& key)
        : info{ name, key, "", QMetaType::QVariant, {}, "", {}, {}, {} } {
    }

    PropertyInfo info;
};

#endif // PROPERTYINFO_H