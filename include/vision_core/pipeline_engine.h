#ifndef PIPELINE_ENGINE_H
#define PIPELINE_ENGINE_H

#include <QObject>
#include <QVector>
#include <QFuture>
#include <QtConcurrent>
#include <atomic>
#include "ialgorithm.h"
#include "algorithm_context.h"

class VISION_CORE_EXPORT PipelineEngine : public QObject
{
    Q_OBJECT
public:
    explicit PipelineEngine(QObject* parent = nullptr);
    ~PipelineEngine();

    void addStep(std::shared_ptr<IAlgorithm> algo);
    void clearSteps();
    int stepsCnt();
    QVector<std::shared_ptr<IAlgorithm>> algos();
    bool initializeAll();
    void runAsync(AlgorithmContext& context);

signals:
    void stepFinished(int index, const QString& algoName, const AlgorithmResult& result);
    void pipelineFinished(bool success, const AlgorithmContext& finalContext);
    void pipelineStarted();

private:
    void executePipeline(AlgorithmContext context);

    QVector<std::shared_ptr<IAlgorithm>> steps_;
    std::atomic<bool> isRunning_{ false };
};

#endif