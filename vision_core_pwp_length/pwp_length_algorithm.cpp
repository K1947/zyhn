#include "pwp_length_algorithm.h"
#include "property_editor_widget.h"
#include "result_shape.h" // 用于画图
#include "logger_utils.h"

PwpLengthAlgorithm::PwpLengthAlgorithm() :
    prop_obj_(new ConfigurableObject()),
    init_param_status_(false)
{
    setupProperties();
}

PwpLengthAlgorithm::~PwpLengthAlgorithm()
{
}

bool PwpLengthAlgorithm::initialize()
{
    return true;
}

QString PwpLengthAlgorithm::name() const
{
    // 这里的名字要和 Plugin 中的 name() 保持一致
    return "pwp_length";
}

void PwpLengthAlgorithm::setupProperties()
{
    // --- 在这里定义你的参数 ---

    PropertyBuilder::create("选择图像源", "image_source")
        .category("基本参数")
        .type(QMetaType::QVariantMap)
        .enums({ {"图像1", ""} })
        .defaultValue("图像1")
        .registerTo(prop_obj_);

    // 示例参数：你可以改成任何你需要的
    PropertyBuilder::create("测试阈值", "my_threshold")
        .category("算法参数")
        .type(QMetaType::Int)
        .defaultValue(128)
        .registerTo(prop_obj_);

    prop_obj_->registerBtn(1, "测试运行");
}

ConfigurableObject* PwpLengthAlgorithm::getPropertyObj()
{
    return prop_obj_;
}

void PwpLengthAlgorithm::setParameters(const QJsonObject& p)
{
    prop_obj_->setPropetyFromJson(p);
    init_param_status_ = true;
}

void PwpLengthAlgorithm::setRunParameters(const QMap<QString, QList<QPair<QString, QVariant>>>& p)
{
    for (auto it = p.begin(); it != p.end(); it++) {
        prop_obj_->setPropertyEnumValues(it.key(), it.value());
    }
}

// ---------------------------------------------------------
// 核心逻辑：这里是你填入 Halcon 代码的地方
// ---------------------------------------------------------
AlgorithmResult PwpLengthAlgorithm::run(const AlgorithmContext& context)
{
    return test(context.input);
}

AlgorithmResult PwpLengthAlgorithm::test(const AlgorithmInput& input)
{
    AlgorithmResult result;

    // 1. 获取输入图像
    // 注意：需要检查索引是否越界
    if (input.imageCount() <= 0) {
        result.setCode(-1);
        result.setMsg("没有输入图像");
        return result;
    }

    // 获取第0张图转为 Halcon 对象
    HalconCpp::HObject hImg = input.getImageAsHObject(0);
    if (!hImg.IsInitialized()) {
        result.setCode(-1);
        result.setMsg("图像无效");
        return result;
    }

    // 2. 获取参数
    int thresholdVal = prop_obj_->propValue("my_threshold").toInt();

    // 3. 执行你的算法逻辑 (这里是示例：画一个固定的矩形)
    // TODO: 把你的 Halcon 代码粘贴在这里

    // 示例：简单创建一个结果形状
    auto rect = std::make_shared<ResultRect>(QRectF(100, 100, 200, 150), Qt::green);
    result.addResultShape(rect);

    QString msg = QString("pwp_length 算法运行成功，阈值: %1").arg(thresholdVal);
    result.setMsg(msg);
    result.setResultType(ResultType::OK);

    return result;
}