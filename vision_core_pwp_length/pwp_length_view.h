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

    virtual void onPageActivated(const QVariant& data);
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
    PwpLengthAlgorithm* algo_instance_;
    PropertyEditorWidget* property_editor_;
    QStringList page_ids_;
    QString page_id_;
};

#endif // PWP_LENGTH_VIEW_H