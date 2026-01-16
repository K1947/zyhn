#pragma once

#include <QObject>
#include "zyhn_api_global.h"

namespace zyhn_core
{
	class IObject;
	class ZYHN_API_EXPORT IExtension
	{
	public:
		virtual ~IExtension() {}
		virtual void addObject(const QString& meta, QObject* obj) = 0;
		virtual void removeObject(const QString& meta) = 0;
		virtual QObject* findObject(const QString& meta) const = 0;
		virtual QStringList objectMetaList() const = 0;
	};

	class ZYHN_API_EXPORT IObject : public QObject
	{
		Q_OBJECT
	public:
		explicit IObject(QObject* parent = nullptr) : QObject(parent) {}
		virtual ~IObject() = default;
		virtual IExtension* extension() { return 0; }
	};

	template <typename T>
	inline T findExtensionObject(IObject* obj, const QString& meta)
	{
		IExtension* ext = obj->extension();
		if (!ext) {
			return 0;
		}
		QObject* t = ext->findObject(meta);
		if (!t) {
			return 0;
		}
		return static_cast<T>(t);
	}


	template <typename T>
	inline T findExtensionObject(IExtension* ext, const QString& meta)
	{
		QObject* t = ext->findObject(meta);
		if (!t) {
			return 0;
		}
		return static_cast<T>(t);
	}

}
