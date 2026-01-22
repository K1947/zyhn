#pragma once
#ifndef SHAPE_BASED_MATCHING_DEMO_H
#define SHAPE_BASED_MATCHING_DEMO_H

#include "shapematch.h"
#include <string>
#include <vector>


class ShapeBasedMatchingDemo {
public:
	// 运行所有测试
	static void runAllTests();

	// 单个功能测试
	static void testTemplateCreation();

	static void testImagePreprocessing();

	static void testSingleTemplateMatching();

	static void testAdvancedTemplateMatching();

	static void testMultipleTemplates();

	static void testFindAllTemplates();

	static void testTemplateManagement();

	static void testTemplatePersistence();

	static void testBatchOperations();

private:
	// 辅助函数
	static HalconCpp::HObject createCircleImage(int width, int height, int radius);

	static HalconCpp::HObject createRectangleImage(int width, int height, int rectWidth, int rectHeight);

	static HalconCpp::HObject createTriangleImage(int width, int height);

	static HalconCpp::HObject createSearchImage(const std::vector<HalconCpp::HObject>& shapes);

	static void printResultSummary(const std::vector<MatchingResult>& results);

	static bool compareResults(const MatchingResult& r1, const MatchingResult& r2, double tolerance = 1.0);

	// 基础模板测试 ---- 高级模板创建测试
	static void baseTemplateCreate(ShapeBasedMatching& matcher, const HObject& srcImg, const HObject roiRegion);

	static void advancedTemplateCreate(ShapeBasedMatching& matcher, const HObject& srcImg, const HObject roiRegion);

};


#endif		// SHAPE_BASED_MATCHING_DEMO_H
