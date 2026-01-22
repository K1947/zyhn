#ifndef PWP_LENGTH_VIEW_H
#define PWP_LENGTH_VIEW_H

#include "algo_content_view.h"
#include "page_base.h"
#include "pwp_length_algorithm.h"
#include "property_editor_widget.h"

class PwpLengthView : public PageBase
{
    Q_OBJECT
public:
    PwpLengthView(AlgoPageManager* pageMgr, QString type, QWidget* parent = nullptr);
    ~PwpLengthView();

    virtual void onPageActivated(const QVariant& data) override;
    void clearResults();

    IAlgorithm* algorithm() const { return algo_instance_; }

    // 【关键】保存和加载参数的接口
    virtual QJsonObject algoParameters() const override;
    virtual void setAlgoParameters(const QJsonObject& obj) override;

    // 【关键】保存模板图像的接口
    virtual void saveImage() override;

private slots:
    void onPropBtnClickedSlots(QString);
    void onPageChangedSlots(QString);
    void onPropValueChanged(QString key, QVariant value);

private:
    void setImageSourceProp();
    void setImageSource();
    void setImageSourcePropEnums(QStringList list);

private:
    AlgoContentView* algo_content_view_;
    PwpLengthAlgorithm* algo_instance_;
    PropertyEditorWidget* property_editor_;
    QStringList page_ids_;
    QString page_id_;
    bool save_model_img_; // 标记是否需要保存模板图
};

#endif // PWP_LENGTH_VIEW_H