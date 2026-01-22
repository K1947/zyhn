#pragma once
#ifndef SHAPE_BASED_MATCHING_H
#define SHAPE_BASED_MATCHING_H

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <atomic>
#include <mutex>
#include "Halconcpp.h"
#include <cmath>


/*
1. Halcon匹配算法封装功能
算法功能：
a. 创建模板（从图像中创建，并可以设置模板参数）
b. 图像预处理（可以根据需要设置预处理步骤，例如灰度化、滤波等）
c. 获取图像模板的轮廓
d. 模板匹配（可以在图像中匹配多个模板，并返回每个匹配实例的位置、角度和分数）
e. 返回匹配结果（包含坐标、角度和分数）
*/

// 匹配结果结构体
const double m_PI = 3.14159265358979323846;

struct MatchingResult {
	int templateId;					// 模板Id
	double row;						// 行坐标,考虑精度
	double column;					// 列坐标
	double angle;					// 旋转角度（弧度）
	double score;					// 匹配分数
	double scale;					// 缩放比例
	std::string templateName;		// 模板名称
	HalconCpp::HTuple homography;	// 单应性矩阵（用于仿射变换）
	// 构造函数----默认构造
	MatchingResult() : templateId(-1), row(0), column(0), angle(0), score(0), scale(1.0) {}
	// 获取旋转角度（度）;
	double getAngleDegrees() const {
		return angle * 180.0 / m_PI;
	}
};

// 形状匹配相应的模板配置
struct TemplateConfig {
	std::string tmpName;		// 模板名称
	int numLevels;				// 金字塔层数
	double angleStart;			// 起始角度（弧度）
	double angleExtent;			// 角度范围（弧度）
	double angleStep;			// 角度步长
	std::string optimization;	// 优化模式
	std::string metric;			// 对比度量方式
	int contrast;				// 对比度
	int minContrast;			// 最小对比度
	double minScale;			// 最小缩放比例
	double maxScale;			// 最大缩放比例
	bool isScaleInvariant;		// 是否尺寸不变
	HalconCpp::HTuple modelId;	// Halcon模型Id
		
	// 设置默认构造函数
	TemplateConfig() :
		tmpName(""),
		numLevels(0),
		angleStart(-0.39),
		angleExtent(0.79),
		angleStep(0.0174533),		// 约1度
		optimization("auto"),
		metric("use_polarity"),
		contrast(10),
		minContrast(5),
		minScale(0.9),
		maxScale(1.1),
		isScaleInvariant(false) { }
};

class ShapeBasedMatching {
	public:
		// 构造函数
		ShapeBasedMatching();
		// 析构函数
		~ShapeBasedMatching();

		// 禁用拷贝构造和赋值
		ShapeBasedMatching(const ShapeBasedMatching&) = delete;

		ShapeBasedMatching& operator=(const ShapeBasedMatching&) = delete;
		
		// ===================== 模板创建方法 ====================== //
		/*
			@brief 创建单个模板（基础版本）
		*/
		int createTemplate(
			const HalconCpp::HObject& image,
			const HalconCpp::HObject& region,
			const std::string& templateName
		);

		/*
			@brief 创建单个模板（高级版本，带完整配置）
		*/
		int createTemplateAdvanced(
			const HalconCpp::HObject& image,
			const HalconCpp::HObject& region,
			const TemplateConfig& config
		);

		/*
			@brief 批量创建模板
		*/
		std::vector<int> createTemplatesBatch(
			const std::vector<HalconCpp::HObject>& images,
			const std::vector<HalconCpp::HObject>& regions,
			const std::vector<std::string>& names,
			const TemplateConfig& commonConfig = TemplateConfig()
		);

		// ================== 图像预处理方法（枚举处理） ==================== //
		enum PreprocessMethod {
			NONE = 0,
			GRAYSCALE = 1,
			MEDIAN_FILTER = 2,
			GAUSSIAN_FILTER = 3,
			SOBEL_EDGE = 4,
			LAPLACIAN = 5,
			HISTOGRAM_EQUALIZATION = 6,
			MEAN_FILTER = 7,
			BILATERAL_FILTER = 8
		};

		bool preprocessImage(
			const HalconCpp::HObject& image,
			HalconCpp::HObject& processedImage,
			PreprocessMethod method = NONE,
			double pam1 = 3.0,
			double pam2 = 3.0
		);

		// ============================= 模板匹配方法 ========================== //
		/*
			@brief 单个模板匹配 （基础版本）
		*/
		bool findTemplate(
			const HalconCpp::HObject& image,
			int templateId,
			std::vector<MatchingResult>& results,
			double minScore = 0.5,
			int maxMatches = 1,
			double greediness = 0.8
		);

		/*
			@brief 单个模板匹配（高级版本）
		*/
		bool findTemplateAdvanced(
			const HalconCpp::HObject& image,
			int templateId,
			std::vector<MatchingResult>& results,
			const HalconCpp::HTuple& minScore = 0.5,
			int maxMatches = 1,
			const HalconCpp::HTuple& greediness = 0.8,
			const HalconCpp::HTuple& subPixel = "least_squares",
			int numLevels = 0,
			double maxOverlap = 0.5,
			bool isSubpixel = true
		);

		/*
			@brief: 多个模板匹配（优化版本）
		*/
		bool findMultipleTemplatesOptimized(
			const HalconCpp::HObject& image,
			const std::vector<int>& templateIds,
			std::vector<MatchingResult>& results,
			double minScore = 0.5,
			int maxMatchesPerTemplate = 3,
			double greediness = 0.8,
			bool usePyramid = true,
			bool useCache = true,
			int numThreads = 1
		);

		/*
			@brief 获取所有模板的匹配结果（整合版本）
		*/
		bool findAllTemplates(
			const HalconCpp::HObject& image,
			std::vector<MatchingResult>& results,
			double minScore = 0.5,
			int maxMatchesPerTemplate = 3,
			bool useParallel = false
		);

		// ============================== 模板管理方法 ===================== //
		/*
			@brief 获取模板轮廓
		*/
		bool getTemplateContour(
			int templateId,
			HalconCpp::HObject& contour,
			int level = 1
		) const;

		/*
			@brief 获取模板配置信息
		*/
		bool getTemplateConfig(int templateId, TemplateConfig& config) const;

		/*
			@brief 更新模板配置
		*/
		bool updateTemplateConfig(int templatedId, const TemplateConfig& newConfig);

		/*
			@brief 清除指定模板
		*/
		bool clearTemplate(int templateId);

		/*
			@brief 清除所有模板
		*/
		void clearAllTemplates();

		/*
			@brief 获取模板数量
		*/
		size_t getTemplateCount() const;

		/*
			@brief 检查模板是否存在
		*/
		bool templateExists(int templateId) const;

		/*
			@brief 获取模板名称
		*/
		std::string getTemplateName(int templateId) const;

		// ========================== 模板持久化方法 ====================================== //
		/*
			@brief 保存模板到文件
		*/
		bool saveTemplate(int templateId, const std::string& filePath) const;

		/*
			@brief 从文件加载模板
		*/
		int loadTemplate(const std::string& filePath, const std::string& templateName);

		/*
			@brief 保存所有模板
		*/
		bool saveAllTemplates(const std::string& directoryPath) const;

		/*
			@brief 从目录加载所有模板
		*/
		std::vector<int> loadAllTemplates(const std::string& directoryPath);

	private:
		// 模板信息结构定义 （放在类的私有部分）
		struct TemplateInfo {
			int id;
			std::string name;
			HalconCpp::HTuple modelId;
			HalconCpp::HObject contour;
			double angleStart;
			double angleExtent;

			TemplateInfo(int id_, const std::string& name_,
				const HalconCpp::HTuple& modelId_,
				double angleStart_, double angleExtent_) :
				id(id_), name(name_), modelId(modelId_),
				angleStart(angleStart_), angleExtent(angleExtent_) {
			}
		};

		// 使用typedef定义类型别名，避免复杂的模板语法
		typedef std::shared_ptr<TemplateInfo> TemplateInfoPtr;
		typedef std::map<int, TemplateInfoPtr> TemplateMap;

		// 线程安全的模板储存
		TemplateMap templates_;
		mutable std::mutex templatesMutex_;

		// 下一个可用的模板ID
		std::atomic<int> nextTemplateId_;
		// 线程池（简化实现）
		int numThreads_;

		// ========================= 私有辅助方法 ============================== //
		/*
			@brief 内部查找模板信息（线程安全）
		*/
		TemplateInfoPtr findTemplateInfo(int templateId) const;

		/*
			@brief 获取下一个可用的模板ID
		*/
		int getNextTemplateId();

		/*
			@brief 执行halcon匹配 （核心方法）
		*/
		bool executeHalconMatch(
			const HalconCpp::HObject& image,
			const TemplateInfo& templateInfo,
			std::vector<MatchingResult>& results,
			const HalconCpp::HTuple& minScore,
			int maxMatches,
			const HalconCpp::HTuple& greediness,
			const HalconCpp::HTuple& subPixel,
			int numLevels,
			double maxOverlap,
			bool isSubpixel
		) const;

		/*
			@brief 转换Halcon结果到MatchingResult
		*/
		void convertHalconResults(
			const HalconCpp::HTuple& rows,
			const HalconCpp::HTuple& cols,
			const HalconCpp::HTuple& angles,
			const HalconCpp::HTuple& scores,
			const HalconCpp::HTuple& scales,
			const TemplateInfo& templateInfo,
			std::vector<MatchingResult>& results
		) const;
};

#endif		// SHAPE_BASED_MATCHING_H