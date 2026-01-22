#include "circlefit_plugin.h"
#include "circlefit_algorithm.h"
#include "circle_fitter_view.h"


std::shared_ptr<IAlgorithm> CircleFitAlgorithmPlugin::createInstance()
{
    return std::make_shared<CircleFitAlgorithm>();
}

PageBase* CircleFitAlgorithmPlugin::createPageView(const QString& name, AlgoPageManager* manager, QWidget* parent)
{
    return new CircleFitterView(manager, name, parent);
}
