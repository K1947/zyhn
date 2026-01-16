#ifndef RESULT_SHAPE_H
#define RESULT_SHAPE_H

#include <QDataStream>
#include <QRectF>
#include <QVector>
#include <QJsonObject>
#include <QFont>
#include <QColor>
#include <QPainterPath>
#include "image_editor_global.h"

enum class ResultType {
    OK,
    NG
};

class IMAGE_EDITOR_EXPORT ResultShape
{
public:
    ResultShape();
    virtual ~ResultShape() {}

    enum ShapeType {
        None,
        Rectangle,      // 矩形
        RoundRetangle,  // 圆角矩形
        Circle,         // 圆形
        ArcRing,        // 弧形
        Polygon,        // 多边形
        Ring,           // 圆环
        Text,
        Path,
        Coordinate,
        Line
    };

    virtual ShapeType type() = 0;
    virtual QRectF boundingRect() const = 0;
    virtual ResultShape* clone() = 0;
    virtual void setColor(const QColor&);
    virtual QColor color() const;
private:
    QColor color_;
};

class IMAGE_EDITOR_EXPORT ResultRect : public ResultShape
{
public:
    ResultRect(QRectF rect, const QColor& color = Qt::blue);
    ResultRect(QPointF topLeft, qreal w, qreal h, const QColor& color = Qt::blue);
    ResultRect(QPointF topLeft, QPointF topRight, QPointF bottomRight, QPointF bottomLeft, const QColor& color = Qt::blue);
    ~ResultRect() {}

public:
    virtual ShapeType type() override;

    virtual QRectF boundingRect() const override;
    virtual ResultShape* clone() override;
    double width() const;
    double height() const;
private:
    QRectF rect_;
};

class IMAGE_EDITOR_EXPORT ResultRoundRect : public ResultShape
{
public:
    ResultRoundRect() { rect_ = QRectF(0, 0, 0, 0); }
    ResultRoundRect(QPointF topLeft, qreal w, qreal h, qreal cornerRadius, const QColor& color = Qt::blue);
    ResultRoundRect(QPointF topLeft, QPointF topRight, QPointF bottomRight, QPointF bottomLeft, qreal cornerRadius, const QColor& color = Qt::blue);
    ~ResultRoundRect() {}

    // RoiShape interface
public:
    virtual ShapeType type() override;

    virtual QRectF boundingRect() const override;
    virtual ResultShape* clone() override;

    double width() const;
    double height() const;

private:
    QRectF rect_;
    qreal corner_radius_;
};

class IMAGE_EDITOR_EXPORT ResultCircle : public ResultShape
{
public:
    ResultCircle() {
        center_ = QPointF(0, 0);
    }
    ResultCircle(QPointF center, qreal r, const QColor& color = Qt::blue);
    ~ResultCircle() {}

    // RoiShape interface
public:
    virtual ShapeType type() override;
    virtual QRectF boundingRect() const override;
    virtual ResultShape* clone() override;
    QPointF center() const;
    void setCenter(QPointF center);
    qreal radius() const;
    void setRadius(qreal r);
private:
    QPointF center_;
    qreal radius_{ 0.0 };
};

class IMAGE_EDITOR_EXPORT ResultRing : public ResultShape
{
public:
    ResultRing() {
        center_ = QPointF(0, 0);
    }
    ResultRing(QPointF center, qreal innerR, qreal outterR, const QColor& color = Qt::blue);
    ~ResultRing() {}

    // RoiShape interface
public:
    virtual ShapeType type() override;
    virtual QRectF boundingRect() const override;
    virtual ResultShape* clone() override;

    QPointF center() const;
    void setCenter(QPointF center);

    qreal innerRadius() const;
    void setInnerRadius(qreal r);

    qreal outterRadius() const;
    void setOutterRadius(qreal r);
private:
    QPointF center_;
    qreal inner_radius_{ 0.0 };
    qreal outter_radius_{ 0.0 };
};

class IMAGE_EDITOR_EXPORT ResultArcRing : public ResultShape
{
public:
    ResultArcRing() {
        center_ = QPointF(0, 0);
    }
    ResultArcRing(QPointF center, qreal innerR, qreal outterR, qreal startAngle, qreal spanAngle, const QColor& color = Qt::blue);
    ~ResultArcRing() {}

    // RoiShape interface
public:
    virtual ShapeType type() override;
    virtual QRectF boundingRect() const override;
    virtual ResultShape* clone() override;
    QPointF center() const;
    void setCenter(QPointF center);

    qreal innerRadius() const;
    void setInnerRadius(qreal r);

    qreal outterRadius() const;
    void setOutterRadius(qreal r);

    qreal startAngle() const;
    void setStartAngle(qreal startAngle);

    qreal spanAngle() const;
    void setSpanAngle(qreal spanAngle);
private:
    QPointF center_;
    qreal inner_radius_{ 0.0 };
    qreal outter_radius_{ 0.0 };
    qreal start_angle_{ 0.0 };
    qreal span_angle_{ 0.0 };
};

class IMAGE_EDITOR_EXPORT ResultPolygon : public ResultShape
{
public:
    ResultPolygon() {}
    ResultPolygon(QPointF center, QVector<QPointF>pts, const QColor& color = Qt::blue);
    ~ResultPolygon() {}
    virtual ShapeType type() override;
    virtual QRectF boundingRect() const override;
    virtual ResultShape* clone() override;

    QVector<QPointF> getPts()const;
    void setPts(QVector<QPointF>pts);
public:
    QVector<QPointF> pts_;
};

class IMAGE_EDITOR_EXPORT ResultText : public ResultShape
{
public:
    ResultText() {}
    ResultText(QString& text, QPointF pos, int fontSize = 32, const QColor& color = Qt::green);
    ~ResultText() {}
    virtual ShapeType type() override;
    virtual QRectF boundingRect() const override;
    virtual ResultShape* clone() override;

    QString text()const;
    int fontSize() const;
    QPointF textPos() const;
public:
    QString text_;
    int font_size_;
    QPointF text_pos_;
};

class IMAGE_EDITOR_EXPORT ResultPath : public ResultShape
{
public:
    ResultPath() {}
    ResultPath(QPainterPath path, const QColor& color = Qt::green);
    ~ResultPath() {}
    virtual ShapeType type() override;
    virtual QRectF boundingRect() const override;
    virtual ResultShape* clone() override;

    QPainterPath getPath()const;
    void setPath(QPainterPath path);
public:
    QPainterPath path_;
};

class IMAGE_EDITOR_EXPORT ResultCoordinate : public ResultShape
{
public:
    ResultCoordinate() {}
    ResultCoordinate(QPointF pt, qreal angle, const QColor& color = Qt::green);
    ~ResultCoordinate() {}
    virtual ShapeType type() override;
    virtual QRectF boundingRect() const override;
    virtual ResultShape* clone() override;

    QPointF getPoint()const;
    void setPoint(QPointF point);

    qreal getAngle()const;
    void setAngle(qreal angle);
public:
    QPointF pt_;
    qreal angle_;
};

class IMAGE_EDITOR_EXPORT ResultLine : public ResultShape
{
public:
    ResultLine() {}
    ResultLine(QLineF line, const QColor& color = Qt::green);
    ~ResultLine() {}
    virtual ShapeType type() override;
    virtual QRectF boundingRect() const override;
    virtual ResultShape* clone() override;

    QLineF getLine()const;
    void setLine(QLineF point);
public:
    QLineF line_;
};

#endif // ROISHAPE_H




