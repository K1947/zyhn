#ifndef UTILS_H
#define UTILS_H
#include <QObject>
#include <QWidget>
#include "toast.h"
#include "zyhn_api_global.h"

namespace zyhn_core {
    class ZYHN_API_EXPORT Utils: public QObject {
        Q_OBJECT
    public:
        Utils();
        ~Utils();
        static void setStyleSheet(QWidget* wgt, QString filePath, bool isAppend = true);
        static void showToast(QString msg);
        static QString encryptStr(const QString& str);
    };
}

#endif // !UTILS_H

