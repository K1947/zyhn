#ifndef CIRCLE_FITTER_VIEW_H
#define CIRCLE_FITTER_VIEW_H

#include "algo_content_view.h"
#include "page_base.h"
#include "circlefit_algorithm.h"
#include "property_editor_widget.h"


/*
 圆拟合算法参数配置类
*/
class CircleFitterView: public PageBase
{
    Q_OBJECT
public:
    // 构造函数
    CircleFitterView(AlgoPageManager* pageMgr, QString type, QWidget* parent = nullptr);
    // 析构函数
    ~CircleFitterView();
    // 界面激活方法
    virtual void onPageActivated(const QVariant& data);
    // 结果清除函数
    void clearResults();
    // 圆拟合算法
    IAlgorithm* algorithm() const { return circle_fitter_; }
    // 算法参数配置，开放到参数界面参数
    virtual QJsonObject algoParameters() const override;
    // 设置算法参数
    virtual void setAlgoParameters(const QJsonObject& obj) override;
    // 保存图像
    virtual void saveImage() override;

private slots:
    // 视觉导航界面相应按钮槽函数
    void onPropBtnClickedSlots(QString);   
    // 界面响应槽函数
    void onPageChangedSlots(QString);
    // 视觉导航界面值响应槽函数
    void onPropValueChanged(QString key, QVariant value);

private:
    // 私有方法 -- 设置图像源界面
    void setImageSourceProp();
    void setImageSource();
    void setImageSourcePropEnums(QStringList list);

private:
    AlgoContentView* algo_content_view_;
    CircleFitAlgorithm* circle_fitter_;
    PropertyEditorWidget* property_editor_;
    QStringList page_ids_;
    QString page_id_;
    bool save_model_img_;
};

#endif // !CIRCLE_FITTER_VIEW_H