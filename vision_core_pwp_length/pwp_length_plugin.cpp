#include "pwp_length_plugin.h"
#include "pwp_length_algorithm.h"
#include "pwp_length_view.h"

std::shared_ptr<IAlgorithm> PwpLengthAlgorithmPlugin::createInstance()
{
    return std::make_shared<PwpLengthAlgorithm>();
}

PageBase* PwpLengthAlgorithmPlugin::createPageView(const QString& name, AlgoPageManager* manager, QWidget* parent)
{
    return new PwpLengthView(manager, name, parent);
}