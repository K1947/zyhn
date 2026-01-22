#ifndef HAIRY_FABRIC_ALGORITHM_H
#define HAIRY_FABRIC_ALGORITHM_H

#include "ialgorithm.h"
#include "configurable_object.h"
#define NOMINMAX
#include <halconCpp/HalconCpp.h>

class HairyFabricAlgorithm : public IAlgorithm
{
public:
    HairyFabricAlgorithm();
    ~HairyFabricAlgorithm();

    virtual bool initialize() override;
    virtual QString name() const override;

    virtual void setRunParameters(const QMap<QString, QList<QPair<QString, QVariant>>>& p);
    virtual ConfigurableObject* getPropertyObj() override;
    virtual void setParameters(const QJsonObject& p) override;

    virtual AlgorithmResult run(const AlgorithmContext& input) override;
    AlgorithmResult test(const AlgorithmInput& input);

private:
    void setupProperties();

private:
    ConfigurableObject* prop_obj_;
    bool init_param_status_;
};

#endif // HAIRY_FABRIC_ALGORITHM_H