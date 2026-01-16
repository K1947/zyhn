#ifndef TOAST_H
#define TOAST_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QMutex>
#include <QThread>
#include <QMetaObject>
#include "zyhn_api_global.h"

class ZYHN_API_EXPORT Toast : public QWidget
{
    Q_OBJECT
        Q_PROPERTY(float opacity READ opacity WRITE setOpacity)

public:
    enum ToastType {
        Info,
        Success,
        Warning,
        Error
    };

    explicit Toast(QWidget* parent = nullptr);
    ~Toast();

    void setText(const QString& text);
    void setDuration(int msec);
    void setType(ToastType type);
    void showToast();

    float opacity() const;
    void setOpacity(float opacity);

    static void show(const QString& text, int duration = 2000, QWidget* parent = nullptr);
    static void info(const QString& text, int duration = 2000, QWidget* parent = nullptr);
    static void success(const QString& text, int duration = 2000, QWidget* parent = nullptr);
    static void warning(const QString& text, int duration = 2000, QWidget* parent = nullptr);
    static void error(const QString& text, int duration = 3000, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onHideAnimationFinished();
    void onShowAnimationFinished();

private:
    void initUI();
    void startShowAnimation();
    void startHideAnimation();
    void updateStyle();
    QString getTypeColor() const;
    QString getTypeIcon() const;

private:
    QLabel* iconLabel_;
    QLabel* textLabel_;
    QPropertyAnimation* animation_;
    QTimer* timer_;
    int duration_;
    float opacity_;
    ToastType type_;
    QString text_;
};

#endif // TOAST_H