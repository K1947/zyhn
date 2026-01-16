#ifndef READ_CODE_ALGORITHM_H
#define READ_CODE_ALGORITHM_H

#define NOMINMAX
#include <halconCpp/HalconCpp.h>

#include "ialgorithm.h"
#include "configurable_object.h"
#include "property_editor_widget.h"
#include "roi_shape.h"

using namespace HalconCpp;
class ReadCodeAlgorithm: public IAlgorithm
{
public:
    ReadCodeAlgorithm();
    ~ReadCodeAlgorithm();
    virtual bool initialize() override;
    virtual QString name() const override;
    virtual void setRunParameters(const QMap<QString, QList<QPair<QString, QVariant>>>& p);
    virtual void setParameters(const QJsonObject& p) override;
    virtual ConfigurableObject* getPropertyObj() override;
    virtual AlgorithmResult run(const AlgorithmContext& input) override;
    AlgorithmResult test(const AlgorithmInput& input);
private:
    HObject getRoiMaskImage(HObject& image, const QVector<std::shared_ptr<RoiShape>> shapes);
    HTuple corAngle(double angle);
    void setupProperties();
private:
    ConfigurableObject* prop_obj_;
    HalconCpp::HObject hImg;
    bool init_param_status_;
    QVector<std::shared_ptr<RoiShape>> roishapes_;
};


#endif // !READ_CODE_ALGORITHM_H