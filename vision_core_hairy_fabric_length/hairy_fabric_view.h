#ifndef HAIRY_FABRIC_VIEW_H
#define HAIRY_FABRIC_VIEW_H

#include "algo_content_view.h"
#include "page_base.h"
#include "hairy_fabric_algorithm.h"
#include "property_editor_widget.h"

class HairyFabricView : public PageBase
{
    Q_OBJECT
public:
    HairyFabricView(AlgoPageManager* pageMgr, QString type, QWidget* parent = nullptr);
    ~HairyFabricView();

    virtual void onPageActivated(const QVariant& data) override;
    void clearResults();

    IAlgorithm* algorithm() const { return algo_instance_; }

    virtual QJsonObject algoParameters() const override;
    virtual void setAlgoParameters(const QJsonObject& obj) override;
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
    HairyFabricAlgorithm* algo_instance_;
    PropertyEditorWidget* property_editor_;
    QStringList page_ids_;
    QString page_id_;
    bool save_model_img_;
};

#endif // HAIRY_FABRIC_VIEW_H