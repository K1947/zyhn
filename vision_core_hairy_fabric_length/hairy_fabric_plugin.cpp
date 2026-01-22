#include "hairy_fabric_plugin.h"
#include "hairy_fabric_algorithm.h"
#include "hairy_fabric_view.h"

std::shared_ptr<IAlgorithm> HairyFabricPlugin::createInstance()
{
    return std::make_shared<HairyFabricAlgorithm>();
}

PageBase* HairyFabricPlugin::createPageView(const QString& name, AlgoPageManager* manager, QWidget* parent)
{
    return new HairyFabricView(manager, name, parent);
}