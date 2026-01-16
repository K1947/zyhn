#ifndef ROI_ITEM_H
#define ROI_ITEM_H

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QVector>
#include <QCursor>
#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QJsonObject>
#include "image_editor_global.h"
#include "roi_shape.h"

enum class ROIType {
    None,
    Rectangle,       // 矩形
    RoundRectangle,  // 圆角矩形
    Circle,          // 圆形
    ArcRing,         // 弧形
    Polygon,         // 多边形
    Ring,            // 圆环
    Text,            // 文字
    Path,            // 路径
    Coordinate,      // 坐标
    Line             // 线段
};

class ROIItemPrivate;
class IMAGE_EDITOR_EXPORT ROIItem : public QGraphicsItem
{
public:
    explicit ROIItem(QGraphicsItem* parent = nullptr);
    virtual ~ROIItem() = default;

    virtual ROIType getROIType() const = 0;
    virtual QRectF boundingRect() const override = 0;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override = 0;
    virtual std::shared_ptr<RoiShape> toRoiShape() const = 0;
    virtual void fromRoiShape(const std::shared_ptr<RoiShape>&) = 0;
    virtual QPointF center() const;
    virtual qreal innerRadius() const;
    virtual qreal outerRadius() const;

    void setSelected(bool selected);
    bool isSelected() const;
    void setPen(const QPen& pen);
    void setBrush(const QBrush& brush);
    void setIsResizing(bool isResizing);
    bool isResizing();
    void setIsDraging(bool isDraging);
    bool isDraging();
    void setIsRotating(bool isRotating);
    bool isRotating();
    qreal handleSize() const;
    qreal cornerRadius() const;
    QList<QPointF> polygonPoints();

    QPen pen() const;
    QBrush brush() const;

    void setRotation(qreal angle);
    qreal rotation() const;

    // 设置是否可调整大小
    void setResizable(bool resizable);
    bool isResizable() const;

    void setMoveable(bool moveable);
    bool isMoveable() const;

    void setOrigPos(QPointF pos);
    QPointF origPos();

    bool isMask() const;
    void setMask(bool);

    void setPenColor(const QColor& color);
    QColor penColor() const;

    void setWeakPtr(const std::weak_ptr<ROIItem>& w);
    std::shared_ptr<ROIItem> weakToShared() const;

    static constexpr qreal HANDLE_R = 4.0;
    static constexpr qreal PEN_WIDTH = 2.0;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* e) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* e) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* e) override;    
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void setLod(qreal lod);
    qreal lod() const;
        
    ROIItemPrivate* d_ptr;
    Q_DECLARE_PRIVATE(ROIItem)
    Q_DISABLE_COPY(ROIItem)

private:
    std::weak_ptr<ROIItem> self_;
};

#endif // !ROI_ITEM_H