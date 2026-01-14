#include "read_code_plugin.h"
#include "read_code_algorithm.h"
#include "read_code_view.h"

IAlgorithm* ReadCodeAlgorithmPlugin::createInstance()
{
    return new ReadCodeAlgorithm();
}

void ReadCodeAlgorithmPlugin::destroyInstance(IAlgorithm* instance)
{
    delete instance;
}

PageBase* ReadCodeAlgorithmPlugin::createPageView(const QString& type, AlgoPageManager* mgr, QWidget* parent)
{
    return new ReadCodeView(mgr, type, parent);
}