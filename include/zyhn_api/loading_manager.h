#ifndef LOADING_MANAGER_H
#define LOADING_MANAGER_H

#include <QObject>
#include <QString>
#include <QPointer>
#include <QMutex>
#include "zyhn_api_global.h"

class QWidget;
class QLabel;
class QMovie;

class ZYHN_API_EXPORT LoadingManager : public QObject
{
    Q_OBJECT
public:
    static LoadingManager& instance();

    void begin(const QString& message = QString());
    void end();
    void setMessage(const QString& message);
    void setGifPath(const QString& gifPath);
    void setParentWidget(QWidget* parent);
protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
private:
    explicit LoadingManager(QObject* parent = nullptr);
    ~LoadingManager();

    void ensureOverlayCreatedOnGuiThread();
    void updateOverlayGeometry();
    void showOnGuiThread(const QString& message);
    void hideOnGuiThread();
    void setMessageOnGuiThread(const QString& message);

private:
    QMutex mutex_;
    int refCount_ = 0;
    QString pendingMessage_;
    QString gifPath_;
    QPointer<QWidget> parentWidget_;
    QPointer<QWidget> overlay_;
    QPointer<QLabel> gifLabel_;
    QPointer<QLabel> textLabel_;
    QPointer<QMovie> movie_;
};

#endif // LOADING_MANAGER_H
