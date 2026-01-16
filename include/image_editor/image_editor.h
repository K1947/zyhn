#ifndef IMAGE_EDITOR_H
#define IMAGE_EDITOR_H

#include "image_editor_global.h"
#include "roi_item.h"
#include <QWidget>
#include <QJsonArray>
#include "drawing_result_manager.h"
#include "roi_shape.h"
#include "result_shape.h"

class IMAGE_EDITOR_EXPORT ImageEditor: public QWidget
{
    Q_OBJECT
public:
    ImageEditor(QWidget* parent = nullptr);
    ~ImageEditor();
    void setImage(const QImage&);
    void setRoiVisible(bool isVisible);
    void setHeaderVisible(bool isVisible);
    const QImage& getImage() const;
    QVector<std::shared_ptr<ROIItem>> getROIsByType(ROIType type);
    QVector<std::shared_ptr<ROIItem>> getAllROIs();
    void clearAllResult();
    void setMove();
    void setSelect();
    void loadRoi(const QJsonObject& obj);
    QJsonArray getRoiParameters() const;
    void setResultShapes(const QVector<std::shared_ptr<ResultShape>> shapes);
    QVector<std::shared_ptr<RoiShape>> getRoiShapes() const;
    void clearRois();
    void setDetailWindowVisible(bool visible);
    void setResultType(ResultType rt);
    void setResultTypeVisible(bool visible);
    void setPosInfoVisible(bool visible);
private:
    class Impl;
    QScopedPointer<Impl> impl_;
};

#endif // ! IMAGE_EDITOR_H