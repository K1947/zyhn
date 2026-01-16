#ifndef DRAWING_RESULT_MANAGER_H
#define DRAWING_RESULT_MANAGER_H

#include <QObject>
#include <QGraphicsScene>
#include <QVector>
#include "roi_item.h"
#include "result_shape.h"
#include "image_editor_global.h"

class IMAGE_EDITOR_EXPORT DrawingResultManager: public QObject
{
    Q_OBJECT
public:
    explicit DrawingResultManager(QGraphicsScene* scene, QGraphicsView* view, QGraphicsPixmapItem* roiParent = nullptr, QObject* parent = nullptr);
    ~DrawingResultManager();
    std::shared_ptr<ROIItem> createRectangle(const QRectF& rect, const QColor& penColor, QGraphicsItem* parent = nullptr);
    std::shared_ptr<ROIItem> createCircle(const QPointF& center, qreal radius, const QColor& penColor, QGraphicsItem* parent = nullptr);
    std::shared_ptr<ROIItem> createArcRing(const QPointF& center, qreal outRadius, qreal innerRadius, qreal startAngle, qreal spanAngle, const QColor& penColor, QGraphicsItem* parent = nullptr);
    std::shared_ptr<ROIItem> createPolygon(const QVector<QPointF>& points, const QColor& penColor, QGraphicsItem* parent = nullptr);
    std::shared_ptr<ROIItem> createRing(const QPointF& center, qreal outRadius, qreal innerRadius, const QColor& penColor, QGraphicsItem* parent = nullptr);
    std::shared_ptr<ROIItem> createRoundedRect(const QRectF& rect, qreal cornerRadius, const QColor& penColor, QGraphicsItem* parent = nullptr);
    std::shared_ptr<ROIItem> createText(QString text, QPointF pos, int fontSize = 32, const QColor& penColor = Qt::green, QGraphicsItem* parent = nullptr);
    std::shared_ptr<ROIItem> createPath(QPainterPath path, const QColor& penColor, QGraphicsItem* parent = nullptr);
    std::shared_ptr<ROIItem> createCoordinate(QPointF point, qreal angle, const QColor& penColor, QGraphicsItem* parent = nullptr);
    std::shared_ptr<ROIItem> createLine(QLineF line, const QColor& penColor, QGraphicsItem* parent = nullptr);

    void addROI(std::shared_ptr<ROIItem> roi);
    void removeROI(std::shared_ptr<ROIItem> roi);
    void clearAllROIs();
    QVector<std::shared_ptr<ROIItem>> getAllROIs() const;
    QVector<std::shared_ptr<ROIItem>> getROIsByType(ROIType type) const;

    void selectROI(std::shared_ptr<ROIItem> roi);
    void deselectAll();
    std::shared_ptr<ROIItem> getSelectedROI() const;

    void setPen(const QPen& pen);
    void setBrush(const QBrush& brush);
    void setPenForSelected(const QPen& pen);
    void setBrushForSelected(const QBrush& brush);

    void setRoiResult(const QVector<std::shared_ptr<ResultShape>> result);
    QVector<std::shared_ptr<ResultShape>> resultShapes();

signals:
    void roiAdded(ROIItem* roi);
    void roiRemoved(ROIItem* roi);
    void roiSelected(ROIItem* roi);
    void clearAllDrawingResultSignals();
public slots:
    void onDelSelectItemsSlots();
private slots:
    void onROISelectionChanged();

private:
    class Impl;
    QScopedPointer<Impl> impl_;
};

#endif // !DRAWING_RESULT_MANAGER_H