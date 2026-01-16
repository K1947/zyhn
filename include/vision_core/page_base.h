#ifndef PAGE_BASE_H
#define PAGE_BASE_H

#include <QWidget>
#include <QVariant>
#include <QDateTime>
#include <QJsonObject>
#include "vision_core_global.h"

class AlgoPageManager;
class VISION_CORE_EXPORT PageBase : public QWidget
{
    Q_OBJECT
public:
    explicit PageBase(AlgoPageManager* manager, QString pageName, QWidget* parent = nullptr);
    virtual ~PageBase() = default;
    virtual void onPageActivated(const QVariant& data);
    virtual void onPageDeactivated();
    virtual QString pageId() const;
    virtual QString pageName() const;
    virtual QImage getImage() const;    
    virtual QJsonObject algoParameters() const = 0;
    virtual void setAlgoParameters(const QJsonObject& obj) = 0;
    virtual void saveImage() = 0;
    virtual QString modelImgName() const;
protected:
    AlgoPageManager* manager() const;

private:   
    QString page_id_;
    QString page_name_;
    AlgoPageManager* manager_;
};

#endif // PAGEBASE_H