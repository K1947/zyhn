#ifndef READ_CODE_VIEW_H
#define READ_CODE_VIEW_H

#include "algo_content_view.h"
#include "page_base.h"
#include "read_code_algorithm.h"
#include "property_editor_widget.h"

class ReadCodeView : public PageBase
{
    Q_OBJECT
public:
    ReadCodeView(AlgoPageManager* pageMgr, QString type, QWidget* parent = nullptr);
    ~ReadCodeView();
    virtual void onPageActivated(const QVariant& data);
    void clearResults();
    IAlgorithm* algorithm() const { return read_code_; }
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
    PropertyEditorWidget* property_editor_;
    ReadCodeAlgorithm* read_code_;
    QStringList page_ids_;
    QString page_id_;
};

#endif // !READ_CODE_VIEW_H

