#ifndef CRESULT_H
#define CRESULT_H

#include "zyhn_api_global.h"
#include <QString>
#include <QDebug>

namespace zyhn_core {

    class ZYHN_API_EXPORT CResult {
    public:
        CResult();
        CResult(int code, const QString& message = QString());
        explicit CResult(const QString& message);

        constexpr bool isOk() const { return code_ == 0; }
        constexpr bool isError() const { return code_ != 0; }
        explicit operator bool() const { return isError(); }

        constexpr int code() const { return code_; }
        const QString& message() const { return message_; }

        QString toString() const;

        constexpr bool operator==(const CResult& other) const {
            return code_ == other.code_;
        }
        constexpr bool operator==(int code) const {
            return code_ == code;
        }
        constexpr bool operator!=(const CResult& other) const {
            return !(*this == other);
        }
        constexpr bool operator!=(int code) const {
            return !(*this == code);
        }

        static CResult ok();

    private:
        int code_;
        QString message_;
    };

    inline const CResult Ok = CResult();

    inline QDebug operator<<(QDebug debug, const CResult& error) {
        QDebugStateSaver saver(debug);
        debug.nospace() << error.toString();
        return debug;
    }

#define DEFINE_ERROR(Name, Code, Message) \
    constexpr CResult Name(Code, Message)

} // namespace zyhn_core

#endif // CRESULT_H