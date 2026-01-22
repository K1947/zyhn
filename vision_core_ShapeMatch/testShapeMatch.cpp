#include "testShapeMatch.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <sstream>


using namespace HalconCpp;
using namespace std;
using namespace chrono;

// 运行所有测试
void ShapeBasedMatchingDemo::runAllTests()
{
	cout << "====================================" << endl;
	cout << " ShapeBasedMatching 功能测试开始 " << endl;
	cout << "====================================" << endl;
	int testCount = 0;
	int passedCount = 0;

	auto runTest = [&](const string& testName, void(*testFunc)()) {
		cout << "\n[" << ++testCount << "] 测试: " << testName << endl;
		cout << string(50, '-') << endl;
		try {
			testFunc();
			cout << "?" << testName << " 测试通过" << endl;
			passedCount++;
		} catch (const exception& ex) {
			cout << "?" << testName << "测试失败: " << ex.what() << endl;
		}
	};
	// 运行各个测试
	runTest("模板创建功能", testTemplateCreation);
	runTest("图像预处理功能", testImagePreprocessing);
	runTest("单个模板匹配", testSingleTemplateMatching);
	runTest("高级模板匹配", testAdvancedTemplateMatching);
	runTest("多个模板匹配", testMultipleTemplates);
	runTest("查找所有模板", testFindAllTemplates);
	runTest("模板管理功能", testTemplateManagement);
	runTest("模板持久化", testTemplatePersistence);
	runTest("批量操作", testBatchOperations);

	// 输出测试总结
	cout << "\n" << string(50, '=') << endl;
	cout << "测试总结:" << endl;
	cout << "总测试数: " << testCount << endl;
	cout << "通过数: " << passedCount << endl;
	cout << "失败数: " << (testCount - passedCount) << endl;
	cout << "通过率: " << fixed << setprecision(1)
		<< (passedCount * 100.0 / testCount) << "%" << endl;
	if (passedCount == testCount) {
		cout << "\n 所有测试通过!" << endl;
	} else {
		cout << "\n 有" << (testCount - passedCount) << "个测试失败" << endl;
	}
 }

void ShapeBasedMatchingDemo::baseTemplateCreate(ShapeBasedMatching& matcher, const HObject& srcImg, const HObject roiRegion)
{
	// 1. 测试基础模板创建
	cout << "1. 基础模板创建测试:" << endl;
	int basicTemplateId = matcher.createTemplate(srcImg, roiRegion, "CircleTemplate");
	if (basicTemplateId == -1) {
		throw runtime_error("基础模板创建失败");
	}
	if (!matcher.templateExists(basicTemplateId)) {
		throw runtime_error("模板存在性检查失败");
	}

	if (matcher.getTemplateName(basicTemplateId) != "CircleTemplate") {
		throw runtime_error("模板名称不匹配");
	}

	cout << " 基础模板创建成功: ID=" << basicTemplateId << ", 名称=" <<
		matcher.getTemplateName(basicTemplateId) << endl;
}

void ShapeBasedMatchingDemo::advancedTemplateCreate(ShapeBasedMatching& matcher, const HObject& srcImg, const HObject roiRegion)
{
	cout << "\n2.高级模板创建测试: " << endl;
	TemplateConfig config;
	config.tmpName = "AdvancedCircleTemplate";
	config.numLevels = 3;
	config.angleStart = -1.0;		// -57度
	config.angleExtent = 2.0;		// 114度
	config.angleStep = 0.1;			// 约5.7度
	config.optimization = "point_reduction_medium";
	config.metric = "use_polarity";
	config.contrast = 20;
	config.minContrast = 10;
	config.isScaleInvariant = true;
	config.minScale = 0.8;
	config.maxScale = 1.2;

	int advancedTemplateId = matcher.createTemplateAdvanced(srcImg, roiRegion, config);
	if (advancedTemplateId == -1) {
		throw runtime_error("高级模板创建失败");
	}
	TemplateConfig retrievedConfig;
	if (!matcher.getTemplateConfig(advancedTemplateId, retrievedConfig)) {
		throw runtime_error("获取模板配置失败");
	}
	// 验证配置
	if (retrievedConfig.tmpName != config.tmpName ||
		retrievedConfig.numLevels != config.numLevels ||
		fabs(retrievedConfig.angleStart - config.angleStart) > 0.01 ||
		fabs(retrievedConfig.angleExtent - config.angleExtent) > 0.01) {
		throw runtime_error("模板配置不匹配");
	}
	cout << " 高级模板创建成功：ID=" << advancedTemplateId << ", 名称=" <<
		retrievedConfig.tmpName << endl;
	// 测试获取模板轮廓
	cout << "\n 3.模板轮廓测试:" << endl;
	HObject contour;
	if (!matcher.getTemplateContour(advancedTemplateId, contour)) {
		throw runtime_error("获取模板轮廓失败");
	}
	HTuple count;
	CountObj(contour, &count);
	if (count[0].I() == 0) {
		throw runtime_error("模板轮廓为空");
	}
	cout << " 模板轮廓获取成功，轮廓对象数: " << count[0].I() << endl;
}



// 测试1：模板创建功能
void ShapeBasedMatchingDemo::testTemplateCreation()
{
	ShapeBasedMatching matcher;
	// 创建测试图像和区域
	HObject circleImage = createCircleImage(200, 200, 50);
	HObject circleRegion;
	GenCircle(&circleRegion, 100, 100, 50);
	// 1. 基础模板创建测试
	baseTemplateCreate(matcher, circleImage, circleRegion);
	// 2. 高级模板创建测试
	advancedTemplateCreate(matcher, circleImage, circleRegion);
}

// 测试2：图像预处理功能
void ShapeBasedMatchingDemo::testImagePreprocessing()
{
	ShapeBasedMatching matcher;
	// 创建测试图像
	HObject testImage = createRectangleImage(300, 300, 100, 150);
	cout << "图像预处理方法测试:" << endl;

	// 测试所有预处理方法
	vector<pair<ShapeBasedMatching::PreprocessMethod, string>> methods = {
		{ ShapeBasedMatching::NONE, "无预处理"},
		{ShapeBasedMatching::GRAYSCALE, "灰度化"},
		{ShapeBasedMatching::MEDIAN_FILTER, "中值滤波"},
		{ShapeBasedMatching::GAUSSIAN_FILTER, "高斯滤波"},
		{ShapeBasedMatching::SOBEL_EDGE, "Sobel边缘检测"},
		{ShapeBasedMatching::LAPLACIAN, "Laplacian滤波"},
		{ShapeBasedMatching::HISTOGRAM_EQUALIZATION, "直方图均衡化"},
		{ShapeBasedMatching::MEAN_FILTER, "均值滤波"}
	};
	
	for (const auto& method : methods) {
		HObject processedImage;
		bool success = matcher.preprocessImage(testImage, processedImage, method.first);
		if (!success) {
			throw runtime_error(method.second + "预处理失败");
		}
		// 检查图像是否有效
		HTuple width, height;
		GetImageSize(processedImage, &width, &height);
		if (width[0].L() == 0 || height[0].L() == 0) {
			throw runtime_error(method.second + " 处理后图像尺寸无效");
		}
		cout << " " << method.second << ":成功(" << width[0].L() << "x"
			<< height[0].L() << ")" << endl;
	}
}

// 测试3：单个模板匹配



