#ifndef ALGO_DATA_MANAGER_H
#define ALGO_DATA_MANAGER_H

#include "zyhn_core.h"
#include <QJsonObject>
#include <QJsonArray>
#include "vision_core_global.h"

using namespace zyhn_core;

class VISION_CORE_EXPORT AlgoDataManager: public IAlgoDataManager
{
    Q_OBJECT
public:
    AlgoDataManager(QObject* parent = nullptr);
    ~AlgoDataManager();
    bool initWithApp(IApplication* app);
    virtual bool saveAlgoParam(const QJsonArray& info, int id) override;
    virtual bool updateAlgo(const QJsonObject& info, int id) override;
    virtual bool deleteAlgo(int id) override;
    virtual QJsonArray getAlgoParam(int pid) override;
private slots:
    void onProductDelSlots(int);
    void onProductAddSlots(int);
private:
    IAppDatabase* db_;
    IApplication* app_;
    static const QString TABLE_NAME;
};

#endif // !ALGO_DATA_MANAGER_h