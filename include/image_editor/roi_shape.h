#ifndef ROISHAPE_H
#define ROISHAPE_H
#include <QDataStream>
#include <QRectF>
#include <QVector>
#include <QJsonObject>
#include "image_editor_global.h"

class IMAGE_EDITOR_EXPORT RoiShape
{
    friend IMAGE_EDITOR_EXPORT QDataStream& operator<<(QDataStream& stream, const RoiShape* shape);
    friend IMAGE_EDITOR_EXPORT QDataStream& operator>>(QDataStream& stream, RoiShape* shape);
public:
    RoiShape();
    virtual ~RoiShape() {}

    enum ShapeType {
        Rectangle,      // 矩形
        RoundRectangle,  // 圆角矩形
        Circle,         // 圆形
        ArcRing,        // 弧形
        Polygon,        // 多边形
        Ring,           // 圆环
    };

    virtual ShapeType type() = 0;

    /**
     * @brief 获取最小外接矩形
     * @return
     */
    virtual QRectF boundingRect() const = 0;
    virtual RoiShape* clone() = 0;
    virtual QJsonObject toJson() const = 0;
    static std::shared_ptr<RoiShape> fromJson(const QJsonObject& obj);

    virtual void transform(double tx, double ty, double ox, double oy, double theta) = 0;
    bool isMasked() const;
    void setMasked(bool mask);

protected:
    /**
     * @brief 将变量写入到指定的数据流
     * @param stream
     */
    virtual void save(QDataStream& stream) const = 0;

public:
    /** 此函数的访问属性应该是protected,但为了兼容之前的版本，需要将此函数的访问属性公开
     * @brief 从指定的数据流读取变量
     * @param stream
     */
    virtual void load(QDataStream& stream) = 0;
private:
    bool mask_ { false };
};

class IMAGE_EDITOR_EXPORT RoiRect : public RoiShape
{
public:
    RoiRect() {
        m_vertex[0] = QPointF(0, 0);
        m_vertex[1] = QPointF(0, 0);
        m_vertex[2] = QPointF(0, 0);
        m_vertex[3] = QPointF(0, 0);
    }
    RoiRect(QPointF topLeft, qreal w, qreal h);
    RoiRect(QPointF topLeft, QPointF topRight, QPointF bottomRight, QPointF bottomLeft);
    ~RoiRect() {}

    // RoiShape interface
public:
    virtual ShapeType type() override;

    virtual QRectF boundingRect() const override;
    virtual RoiShape* clone() override;
    virtual void transform(double tx, double ty, double ox, double oy, double theta) override;

    QPointF vertex(int i) const;
    void setVertex(int i, QPointF pt);

    double width() const;
    double height() const;
    virtual QJsonObject toJson() const override;
    static std::shared_ptr<RoiShape> fromJson(const QJsonObject& obj);
protected:
    virtual void save(QDataStream& stream) const override;    
public:
    virtual void load(QDataStream& stream) override;
private:
    QPointF m_vertex[4];
};

class IMAGE_EDITOR_EXPORT RoiRoundRect : public RoiShape
{
public:
    RoiRoundRect() {
        m_vertex[0] = QPointF(0, 0);
        m_vertex[1] = QPointF(0, 0);
        m_vertex[2] = QPointF(0, 0);
        m_vertex[3] = QPointF(0, 0);
        corner_radius_ = 10.0;
    }
    RoiRoundRect(QPointF topLeft, qreal w, qreal h, qreal cornerRadius);
    RoiRoundRect(QPointF topLeft, QPointF topRight, QPointF bottomRight, QPointF bottomLeft, qreal cornerRadius);
    ~RoiRoundRect() {}

    // RoiShape interface
public:
    virtual ShapeType type() override;

    virtual QRectF boundingRect() const override;
    virtual RoiShape* clone() override;
    virtual void transform(double tx, double ty, double ox, double oy, double theta) override;

    QPointF vertex(int i) const;
    void setVertex(int i, QPointF pt);

    double width() const;
    double height() const;

    qreal cornerRadius() const;
    virtual QJsonObject toJson() const override;
    static std::shared_ptr<RoiShape> fromJson(const QJsonObject& obj);

protected:
    virtual void save(QDataStream& stream) const override;    
public:
    virtual void load(QDataStream& stream) override;
private:
    QPointF m_vertex[4];
    qreal corner_radius_;
};

class IMAGE_EDITOR_EXPORT RoiCircle : public RoiShape
{
public:
    RoiCircle() {
        m_center = QPointF(0, 0);
    }
    RoiCircle(QPointF center, qreal r);
    ~RoiCircle() {}

    // RoiShape interface
public:
    virtual ShapeType type() override;
    virtual QRectF boundingRect() const override;
    virtual RoiShape* clone() override;
    virtual void transform(double tx, double ty, double ox, double oy, double theta) override;

    QPointF center() const;
    void setCenter(QPointF center);

    qreal radius() const;
    void setRadius(qreal r);
    virtual QJsonObject toJson() const override;
    static std::shared_ptr<RoiShape> fromJson(const QJsonObject& obj);
protected:
    virtual void save(QDataStream& stream) const override;    
public:
    virtual void load(QDataStream& stream) override;
private:
    QPointF m_center;
    qreal m_radius{ 0.0 };
};

class IMAGE_EDITOR_EXPORT RoiRing : public RoiShape
{
public:
    RoiRing() {
        m_center = QPointF(0, 0);
    }
    RoiRing(QPointF center, qreal innerR, qreal outterR);
    ~RoiRing() {}

    // RoiShape interface
public:
    virtual ShapeType type() override;
    virtual QRectF boundingRect() const override;
    virtual RoiShape* clone() override;
    virtual void transform(double tx, double ty, double ox, double oy, double theta) override;

    QPointF center() const;
    void setCenter(QPointF center);

    qreal innerRadius() const;
    void setInnerRadius(qreal r);

    qreal outterRadius() const;
    void setOutterRadius(qreal r);

    virtual QJsonObject toJson() const override;
    static std::shared_ptr<RoiShape> fromJson(const QJsonObject& obj);
protected:
    virtual void save(QDataStream& stream) const override;    
public:
    virtual void load(QDataStream& stream) override;
private:
    QPointF m_center;
    qreal m_innerRadius{ 0.0 };
    qreal m_outterRadius{ 0.0 };
};

class IMAGE_EDITOR_EXPORT RoiArcRing : public RoiShape
{
public:
    RoiArcRing() {
        m_center = QPointF(0, 0);
    }
    RoiArcRing(QPointF center, qreal innerR, qreal outterR, qreal startAngle, qreal spanAngle);
    ~RoiArcRing() {}

    // RoiShape interface
public:
    virtual ShapeType type() override;
    virtual QRectF boundingRect() const override;
    virtual RoiShape* clone() override;
    virtual void transform(double tx, double ty, double ox, double oy, double theta) override;

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
    virtual QJsonObject toJson() const override;
    static std::shared_ptr<RoiShape> fromJson(const QJsonObject& obj);
protected:
    virtual void save(QDataStream& stream) const override;
public:
    virtual void load(QDataStream& stream) override;
private:
    QPointF m_center;
    qreal m_innerRadius{ 0.0 };
    qreal m_outterRadius{ 0.0 };
    qreal m_startAngle{ 0.0 };
    qreal m_spanAngle{ 0.0 };
};

class IMAGE_EDITOR_EXPORT RoiPolygon : public RoiShape
{
public:
    RoiPolygon() {}
    RoiPolygon(QPointF center, QVector<QPointF>pts);
    ~RoiPolygon() {}
    virtual ShapeType type() override;
    virtual QRectF boundingRect() const override;
    virtual RoiShape* clone() override;
    virtual void transform(double tx, double ty, double ox, double oy, double theta) override;

    QVector<QPointF>getPts()const;
    void setPts(QVector<QPointF>pts);

    virtual QJsonObject toJson() const override;
    static std::shared_ptr<RoiShape> fromJson(const QJsonObject& obj);

protected:
    virtual void save(QDataStream& stream) const override;    
public:
    virtual void load(QDataStream& stream) override;
public:
    QVector<QPointF>m_pts;
    QPointF m_center;
};
#endif // ROISHAPE_H


