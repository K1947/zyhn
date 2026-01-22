// ShapeBasedMatching.cpp -- 实现文件
#include "shapematch.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <filesystem>
#include <future>


using namespace HalconCpp;
using namespace std;
// ======================== ShapeBasedMatching 类实现 ========================== //
// 构造函数
ShapeBasedMatching::ShapeBasedMatching() : nextTemplateId_(1), numThreads_(1) {
	try {
		// 初始化Halcon并行处理
		SetSystem("parallelize_operators", "true");
		// 设置默认的亚像素模式
		SetSystem("shape_model_subpixel", "least_squares");
		// 启用内部多线程
		SetSystem("tsp_num_threads", "max");
	}
	catch (HException& ex) {
		cerr << "Halcon initialization error: " << ex.ErrorMessage().Text() << endl;
	}
}

ShapeBasedMatching::~ShapeBasedMatching()
{
	// 清理所有模板
	clearAllTemplates();
}

// ========================= 模板创建方法实现 ===================== //

int ShapeBasedMatching::createTemplate(const HalconCpp::HObject& image, const HalconCpp::HObject& region, const std::string& templateName)
{
	TemplateConfig config;
	config.tmpName = templateName;
	config.numLevels = 0;	// 自动计算金字塔层数
	config.angleStart = -0.39;
	config.angleExtent = 0.79;
	config.angleStep = 0.0174533;	// 约1度
	config.optimization = "auto";
	config.metric = "use_polarity";
	config.contrast = 10;
	config.minContrast = 5;
	return createTemplateAdvanced(image, region, config);
}

int ShapeBasedMatching::createTemplateAdvanced(const HalconCpp::HObject& image, const HalconCpp::HObject& region, const TemplateConfig& config)
{
	// 使用互斥锁
	lock_guard<mutex> lock(templatesMutex_);
	try {
		// 检查输入有效性
		if (region.CountObj() == 0) {
			cerr << "Error: Region is empty" << endl;
			return -1;
		}
		// 获取图像尺寸用于验证
		HTuple width, height;
		GetImageSize(image, &width, &height);
		// 创建形状模板
		HTuple modelId;
		// 根据是否支持尺度不变选择不同的创建方法
		if (config.isScaleInvariant && config.minScale > 0 && config.maxScale > 0) {
			// 支持尺度不变性的模板
			CreateScaledShapeModel(
				image,							// 模板图像
				config.numLevels,				// 金字塔层数
				config.angleStart,				// 起始角度
				config.angleExtent,				// 角度范围
				config.angleStep,				// 角度步长
				config.minScale,				// 最小缩放
				config.maxScale,				// 最大缩放
				config.angleStep,				// 缩放步长（使用角度步长）
				config.optimization.c_str(),	// 优化
				config.metric.c_str(),			// 极性
				config.contrast,				// 对比度
				config.minContrast,				// 最小对比度
				&modelId						// 输出模型ID
			);
		} else {
			// 传统形状匹配
			CreateShapeModel(
				image,							// 模板图像
				config.numLevels,				// 金字塔层数
				config.angleStart,				// 起始角度
				config.angleExtent,				// 角度范围
				config.angleStep,				// 角度步长
				config.optimization.c_str(),	// 优化
				config.metric.c_str(),			// 极性
				config.contrast,				// 对比度
				config.minContrast,				// 最小对比度
				&modelId						// 输出模型ID
			);
		}
		// 设置模板原点
		HTuple area, row, col;
		AreaCenter(region, &area, &row, &col);
		if (area.Length() > 0 && area[0].D() > 0) {
			SetShapeModelOrigin(modelId, row[0], col[0]);
		}
		// 获取模板轮廓并缓存
		HObject contour;
		GetShapeModelContours(&contour, modelId, 1);
		// 创建模板信息
		int newId = getNextTemplateId();
		TemplateInfoPtr info = make_shared<TemplateInfo>(newId, config);
		/*info->modelId = modelId;*/
		info->contour = contour;
		// 储存模板
		templates_[newId] = info;
		cout << "Template created successfully: ID=" << newId
			<< ", Name=" << config.tmpName << endl;
		return newId;
	} catch (HException& ex) {
		cerr << "Error creating template: " << ex.ErrorMessage().Text() << endl;
		return -1;
	}
}

std::vector<int> ShapeBasedMatching::createTemplatesBatch(const std::vector<HalconCpp::HObject>& images, const std::vector<HalconCpp::HObject>& regions, const std::vector<std::string>& names, const TemplateConfig& commonConfig)
{
	std::vector<int> createdIds;
	if (images.size() != regions.size() || images.size() != names.size()) {
		cerr << "Error: Input vectors size mismatch" << endl;
		return createdIds;
	}
	// 顺序创建模板（批量创建时保持顺序）
	for (size_t i = 0; i < images.size(); i++) {
		TemplateConfig config = commonConfig;
		config.tmpName = names[i];
		int templateId = createTemplateAdvanced(images[i], regions[i], config);
		if (templateId != -1) {
			createdIds.push_back(templateId);
		}
	}
	return createdIds;
}

// =========================== 图像预处理方法实现 =========================== 
bool ShapeBasedMatching::preprocessImage(const HalconCpp::HObject& image, HalconCpp::HObject& processedImage, PreprocessMethod method, double pam1, double pam2)
{
	try {
		HTuple type,channels;
		GetImageType(image, &type);
		switch (method) {
		case NONE: {
			processedImage = image;
			break;
		}
		case GRAYSCALE: {
			CountChannels(image, &channels);
			if (type.S() == "byte" && channels[0].I() == 3) {
				Rgb1ToGray(image, &processedImage);
			}
			else {
				processedImage = image;
			}
			break;
		}
		case MEDIAN_FILTER: {
			if (type.S() == "byte") {
				MedianImage(image, &processedImage, "circle", pam1, "mirrored");
			}
			else {
				processedImage = image;
			}
			break;
		}
		case GAUSSIAN_FILTER: {
			GaussFilter(image, &processedImage, pam1);
			break;
		}
		case SOBEL_EDGE: {
			HObject gray;
			// 首先检查是否为彩色图像
			CountChannels(image, &channels);
			if (channels[0].I() == 3) {
				Rgb1ToGray(image, &gray);
			}
			else {
				gray = image;
			}
			SobelAmp(gray, &processedImage, "sum_abs", 3);
			break;
		}
		case LAPLACIAN: {
			HObject gray;
			// 首先检查是否为彩色图像
			CountChannels(image, &channels);
			if (channels[0].I() == 3) {
				Rgb1ToGray(image, &gray);
			}
			else {
				gray = image;
			}
			Laplace(gray, &processedImage, "absolute", 3, "n_8");
			break;
		}
		case HISTOGRAM_EQUALIZATION: {
			if (type.S() == "byte") {
				EquHistoImage(image, &processedImage);
			}
			else {
				processedImage = image;
			}
			break;
		}
		case MEAN_FILTER: {
			MeanImage(image, &processedImage, pam1, pam2);
			break;
		}
						//case BILATERAL_FILTER:
						//	// Halcon 没有直接的双边滤波，使用高斯滤波替代
						//	// BilateralFilter(image, image, &processedImage, pam1, pam2, [], []);
						//	break;
		default: {
			processedImage = image;
			break;
		}
		}
		return true;
	} catch (HException& ex) {
		cerr << "Error in preprocessing:" << ex.ErrorMessage().Text() << endl;
		return false;
	}
}

// ================================= 模板匹配方法实现 ==========================//
bool ShapeBasedMatching::findTemplate(const HalconCpp::HObject& image, int templateId, std::vector<MatchingResult>& results, double minScore, int maxMatches, double greediness)
{
	try {
		// 查找模板信息
		TemplateInfoPtr templateInfo = findTemplateInfo(templateId);
		if (!templateInfo) {
			cerr << "Error: Template not found, ID=" << templateId << endl;
			return false;
		}
		// 使用基础参数执行匹配
		HTuple halconMinScore(minScore);
		HTuple halconGreediness(greediness);

		return findTemplateAdvanced(
			image, templateId, results,
			halconMinScore, maxMatches, halconGreediness
		);
	} catch (HException& ex) {
		cerr << "Error in template matching: " << ex.ErrorMessage().Text() << endl;
		return false;
	}
}

bool ShapeBasedMatching::findTemplateAdvanced(const HalconCpp::HObject& image, int templateId, std::vector<MatchingResult>& results, const HalconCpp::HTuple& minScore, int maxMatches, const HalconCpp::HTuple& greediness, const HalconCpp::HTuple& subPixel, int numLevels, double maxOverlap, bool isSubpixel)
{
	try {
		TemplateInfoPtr templateInfo = findTemplateInfo(templateId);
		if (!templateInfo) {
			return false;
		}
		return executeHalconMatch(
			image, *templateInfo, results,
			minScore, maxMatches, greediness,
			subPixel, numLevels, maxOverlap, isSubpixel
		);
	} catch (HException& ex) {
		cerr << "Error in Advanced template matching: " << ex.ErrorMessage().Text() << endl;
		return false;
	}
}

bool ShapeBasedMatching::findMultipleTemplatesOptimized(const HalconCpp::HObject& image, const std::vector<int>& templateIds, std::vector<MatchingResult>& results, double minScore, int maxMatchesPerTemplate, double greediness, bool usePyramid, bool useCache, int numThreads)
{
	results.clear();
	try {
		// 预处理图像
		HObject processedImage;
		preprocessImage(image, processedImage, GRAYSCALE);
		// 确定要搜索的模板
		vector<int> idsToSearch;
		if (templateIds.empty()) {
			lock_guard<mutex> lock(templatesMutex_);
			for (TemplateMap::const_iterator it = templates_.begin(); it != templates_.end(); ++it) {
				idsToSearch.push_back(it->first);
			}
		} else {
			idsToSearch = templateIds;
		}
		
		if (idsToSearch.empty()) {
			cerr << "Warning: No templates to search" << endl;
			return false;
		}
		// 跟据线程数决定匹配策略
		if (numThreads > 1 && idsToSearch.size() > 1) {
			// 并行匹配 - 使用std::async
			vector<future<vector<MatchingResult>>> futures;
			vector<MatchingResult> allResults;
			// 创建异步任务
			for (int templateId : idsToSearch) {
				futures.push_back(async(launch::async, [&, templateId]() {
					vector<MatchingResult> localResults;
					findTemplateAdvanced(
						processedImage, templateId, localResults,
						minScore, maxMatchesPerTemplate, greediness,
						"least_squares", 0, 0.5, true
					);
					return localResults;
					}));
			}
			// 收集结果
			for (size_t i = 0; i < futures.size(); i++) {
				auto templateResults = futures[i].get();
				allResults.insert(allResults.end(),
					templateResults.begin(),
					templateResults.end());
			}
			results = allResults;
		} else {
			// 顺序匹配
			for (int templateId : idsToSearch) {
				vector<MatchingResult> templateResults;
				if (findTemplateAdvanced(processedImage, templateId, templateResults,
					minScore, maxMatchesPerTemplate, greediness,
					"least_squares", 0, 0.5, true)) {
					results.insert(results.end(),
						templateResults.begin(),
						templateResults.end());
				}
			}
		}
		// 按分数排序
		sort(results.begin(), results.end(), [](const MatchingResult& a, const MatchingResult& b) {
			return a.score > b.score;
			});
		// 限制总匹配数量 （防止过多结果）
		size_t maxTotalMatches = maxMatchesPerTemplate * idsToSearch.size();
		if (results.size() > maxTotalMatches) {
			results.resize(maxTotalMatches);
		}
		return !results.empty();
	} catch (HException& ex) {
		cerr << "Error in optimized multiple template matching: "
			<< ex.ErrorMessage().Text() << endl;
		return false;
	}
}

bool ShapeBasedMatching::findAllTemplates(const HalconCpp::HObject& image, std::vector<MatchingResult>& results, double minScore, int maxMatchesPerTemplate, bool useParallel)
{
	return findMultipleTemplatesOptimized(
		image, vector<int>(), results, minScore, maxMatchesPerTemplate,
		0.8, true, true, useParallel ? thread::hardware_concurrency() : 1
	);
}

bool ShapeBasedMatching::getTemplateContour(int templateId, HalconCpp::HObject& contour, int level) const
{
	TemplateInfoPtr templateInfo = findTemplateInfo(templateId);
	if (!templateInfo) {
		return false;
	}
	try {
		if (templateInfo->contour.CountObj() > 0 && level == 1) {
			// 使用缓存的轮廓
			contour = templateInfo->contour;
		} else {
			// 重新获取轮廓
			GetShapeModelContours(&contour, templateInfo->modelId, level);
		}
		return true;
	} catch (HException& ex) {
		cerr << "Error getting template contour: " << ex.ErrorMessage().Text() << endl;
		return false;
	}
}

bool ShapeBasedMatching::getTemplateConfig(int templateId, TemplateConfig& config) const
{
	TemplateInfoPtr templateInfo = findTemplateInfo(templateId);
	if (!templateInfo) {
		return false;
	}
	try {
		// 从Halcon模型获取配置信息
		HTuple angleStart, angleExtent, angleStep, optimization, metric;
		HTuple contrast, minContrast, minScale, maxScale;
		GetShapeModelParams(templateInfo->modelId, &angleStart, &angleExtent,
			&angleStep, &optimization, &metric, &contrast,
			&minContrast, &minScale, &maxScale);
		config.tmpName = templateInfo->name;
		config.angleStart = angleStart.D();
		config.angleExtent = angleExtent.D();
		config.angleStep = angleStep.D();
		config.optimization = optimization.S();
		config.metric = metric.S();
		config.contrast = contrast.I();
		config.minContrast = minContrast.I();
		config.modelId = templateInfo->modelId;
		// 检查是否尺度不变模型
		try {
			// 尝试获取缩放参数，如果失败说明不是尺度不变模型
			HTuple scale;
			GetShapeModelParams(templateInfo->modelId, &HTuple(), &HTuple(),
				&HTuple(), &HTuple(), &HTuple(), &HTuple(),
				&HTuple(), &minScale, &maxScale);
			config.isScaleInvariant = true;
			config.minScale = minScale.D();
			config.maxScale = maxScale.D();
		} catch (...) {
			config.isScaleInvariant = false;
			config.minScale = minScale.D();
			config.maxScale = maxScale.D();
		}
		return true;
	}catch (HException& ex) {
		cerr << "Error getting template config: " << ex.ErrorMessage().Text() << endl;
		return false;
	}
}

bool ShapeBasedMatching::updateTemplateConfig(int templatedId, const TemplateConfig& newConfig)
{
	TemplateInfoPtr templateInfo = findTemplateInfo(templatedId);
	if (!templateInfo) {
		return false;
	}
	try {
		// 更新模板名称
		templateInfo->name = newConfig.tmpName;
		// 注意：Halcon模型参数在创建之后不能直接修改
		// 如果需要修改参数，需要重新创建模板
		cerr << "Warning: Halcon model parameters cannot be modified after creation.";
		return true;
	} catch (HException& ex) {
		return false;
	}
}

bool ShapeBasedMatching::clearTemplate(int templateId)
{
	lock_guard<mutex> lock(templatesMutex_);
	TemplateMap::iterator it = templates_.find(templateId);
	if (it != templates_.end()) {
		ClearShapeModel(it->second->modelId);
		templates_.erase(it);
		cout << "Template cleared: ID= " << templateId << endl;
		return true;
	}
	return false;
}

void ShapeBasedMatching::clearAllTemplates()
{
	lock_guard<mutex> lock(templatesMutex_);
	// 通过循环遍历的方式实现
	for (TemplateMap::iterator it = templates_.begin(); it != templates_.end(); ++it) {
		ClearShapeModel(it->second->modelId);
		templates_.clear();
		cout << "All templates cleared" << endl;
	}
}

size_t ShapeBasedMatching::getTemplateCount() const
{
	lock_guard<mutex> lock(templatesMutex_);
	return templates_.size();
}

bool ShapeBasedMatching::templateExists(int templateId) const
{
	lock_guard<mutex>lock(templatesMutex_);
	return templates_.find(templateId) != templates_.end();
}

std::string ShapeBasedMatching::getTemplateName(int templateId) const
{
	lock_guard<mutex> lock(templatesMutex_);

	TemplateMap::const_iterator it = templates_.find(templateId);
	if (it != templates_.end()) {
		return it->second->name;
	}
	return "";
}

// ============================ 模板持久化方法实现 ================================== //

bool ShapeBasedMatching::saveTemplate(int templateId, const std::string& filePath) const
{
	TemplateInfoPtr templateInfo = findTemplateInfo(templateId);
	if (!templateInfo) {
		return false;
	}
	try {
		WriteShapeModel(templateInfo->modelId, HTuple(filePath.c_str()));
		cout << "Template saved: ID=" << templateId << ", Path=" << filePath << endl;
		return true;
	} catch (HException& ex) {
		cerr << "Error saving template: " << ex.ErrorMessage().Text() << endl;
		return false;
	}
}

int ShapeBasedMatching::loadTemplate(const std::string& filePath, const std::string& templateName)
{
	try {
		HTuple modelId;
		ReadShapeModel(HTuple(filePath.c_str()), &modelId);
		// 获取模板的角度参数
		HTuple numLevel, numLevel, angleStart, angleExtent;
		GetShapeModelParams(modelId, &numLevel, &angleStart, &angleExtent, 
			&HTuple(), &HTuple(), &HTuple(), &HTuple(), &HTuple(), &HTuple());
		// 创建模板信息
		lock_guard<mutex> lock(templatesMutex_);
		int newId = getNextTemplateId();
		TemplateInfoPtr info = make_shared<TemplateInfo>(
			newId, templateName, modelId, angleStart.D(), angleExtent.D());
		// 获取轮廓
		GetShapeModelContours(&(info->contour), modelId, 1);
		// 储存模板
		templates_[newId] = info;
		cout << "Template loaded successfully: ID=" << newId <<
			", Name=" << templateName << endl;
		return newId;
	} catch (HException& ex) {
		cerr << "Error loading template: " << ex.ErrorMessage().Text() << endl;
		return -1;
	}
}

bool ShapeBasedMatching::saveAllTemplates(const std::string& directoryPath) const
{
	try {
		// 创建目录 （如果不存在）
		namespace fs = std::filesystem;
		if (!fs::exists(directoryPath)) {
			fs::create_directories(directoryPath);
		}
		lock_guard<mutex> lock(templatesMutex_);
		bool allSaved = true;
		for (const auto& pair : templates_) {
			int templateId = pair.first;
			const TemplateInfoPtr& info = pair.second;
			string filename = directoryPath + "/template_" + info->name
				+ "_" + to_string(templateId) + ".shm";
			try {
				WriteShapeModel(info->modelId, HTuple(filename.c_str()));
				cout << "Template saved: " << filename << endl;
			} catch (HException& ex) {
				cerr << "Error saving template " << templateId << ": "
					<< ex.ErrorMessage().Text() << endl;
				allSaved = false;
			}
		}
		return allSaved;
	} catch (HException& ex) {
		cerr << "Error saving all templates" << endl;
		return false;
	}
}

std::vector<int> ShapeBasedMatching::loadAllTemplates(const std::string& directoryPath)
{
	std::vector<int> loadedIds;
	try {
		namespace fs = std::filesystem;
		if (!fs::exists(directoryPath)) {
			cerr << "Director does not exist: " << directoryPath << endl;
			return loadedIds;
		}
		// 遍历文件下储存的所有模板
		for (const auto& entry : fs::directory_iterator(directoryPath)) {
			if (entry.path().extension() == ".shm") {
				string filename = entry.path().filename().string();
				// 从文件名提取模板名称
				size_t start = filename.find("template_") + 9;	// "template_"长度
				size_t end = filename.find_last_of("_");
				string templateName = filename.substr(start, end - start);
				int templateId = loadTemplate(entry.path().string(), templateName);
				if (templateId != -1) {
					loadedIds.push_back(templateId);
				}
			}
		}
		return loadedIds;
	} catch (HException& ex) {
		cerr << "Error loading all templates from directory: " << directoryPath << endl;
		return loadedIds;
	}
}

// =================================== 私有辅助方法 ========================== //
ShapeBasedMatching::TemplateInfoPtr
ShapeBasedMatching::findTemplateInfo(int templateId) const
{
	lock_guard<mutex> lock(templatesMutex_);
	TemplateMap::const_iterator it = templates_.find(templateId);
	if (it != templates_.end()) {
		return it->second;
	}
	return TemplateInfoPtr();
}

int ShapeBasedMatching::getNextTemplateId()
{
	return nextTemplateId_++;
}

bool ShapeBasedMatching::executeHalconMatch(const HalconCpp::HObject& image, const TemplateInfo& templateInfo, std::vector<MatchingResult>& results, const HalconCpp::HTuple& minScore, int maxMatches, const HalconCpp::HTuple& greediness, const HalconCpp::HTuple& subPixel, int numLevels, double maxOverlap, bool isSubpixel) const
{
	try {
		HTuple rows, cols, angles, scores, scales;
		// 检查是否是尺度不变模型
		bool isScaled = false;
		HTuple minScale, maxScale;
		// 尝试获取缩放参数
		GetShapeModelParams(templateInfo.modelId, &HTuple(), &HTuple(),
			&HTuple(), &HTuple(), &HTuple(), &HTuple(),
			&HTuple(), &minScale, &maxScale);
		if (minScale[0].D() != 0 && maxScale[0].D() != 0) {
			isScaled = true;
		}
		if (isScaled) {
			// 尺度不变的匹配
			FindScaledShapeModel(
				image,							// 搜索图像
				templateInfo.modelId,	// 模板模型ID
				templateInfo.angleStart,
				templateInfo.angleExtent,
				minScale[0].D(),
				maxScale[0].D(),
				minScore,						// 最小分数
				maxMatches,						// 最大匹配数量
				isSubpixel? 0.5:0.0,		    // 压像素精度
				subPixel,						// 亚像素模式
				greediness,						// 贪婪度
				maxOverlap,						// 最大重叠
				&rows,							// 输出行坐标
				&cols,							// 输出列坐标
				&angles,						// 输出角度
				&scales,						// 输出缩放比例
				&scores							// 输出分数
			);
		} else {
			// 传统形状匹配
			FindShapeModel(
				image,							// 搜索图像
				templateInfo.modelId,	// 模板模型ID
				templateInfo.angleStart,
				templateInfo.angleExtent,
				minScore,						// 最小分数
				maxMatches,						// 最大匹配数量
				isSubpixel? 0.5:0.0,			// 亚像素精度
				subPixel,						// 亚像素模式
				greediness,						// 贪婪度
				maxOverlap,						// 最大重叠
				&rows,							// 输出行坐标
				&cols,							// 输出列坐标
				&angles,						// 输出角度
				&scores							// 输出分数
			);
			// 对于非尺度不变的模型，缩放比例设为1
			scales = HTuple(rows.Length(), 1.0);
		}
		// 转换结果 ---- 转换结果函数
		convertHalconResults(rows, cols, angles, scores, scales, templateInfo, results);
		return !results.empty();
	} catch (HException& ex) {
		cerr << "Error in Halcon matching execution: " << ex.ErrorMessage().Text() << endl;
		return false;
	}
}

void ShapeBasedMatching::convertHalconResults(const HalconCpp::HTuple& rows, const HalconCpp::HTuple& cols, const HalconCpp::HTuple& angles, const HalconCpp::HTuple& scores, const HalconCpp::HTuple& scales, const TemplateInfo& templateInfo, std::vector<MatchingResult>& results) const
{
	results.clear();
	for (int i = 0; i < rows.Length(); i++) {
		MatchingResult result;
		result.templateId = templateInfo.id;
		result.templateName = templateInfo.name;
		result.row = rows[i].D();
		result.column = cols[i].D();
		result.angle = angles[i].D();
		result.score = scores[i].D();
		result.scale = scales.Length() > i ? scales[i].D() : 1.0;

		// 计算单应性矩阵（如果需要）
		if (angles.Length() > 0) {
			VectorAngleToRigid(0, 0, 0, result.row, result.column, result.angle,
				&result.homography);
		}
		results.push_back(result);
	}
}