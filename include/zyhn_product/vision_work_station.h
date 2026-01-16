#ifndef VISION_WORK_STATION_H
#define VISION_WORK_STATION_H

#include <QObject>
#include "product_def.h"
#include <QJsonObject>
#include "zyhn_product_global.h"

namespace zyhn_product {

    class ZYHN_PRODUCT_EXPORT VisionWorkStation : public QObject
    {
        Q_OBJECT
            Q_PROPERTY(int curNo READ curNo WRITE setCurNo NOTIFY curNoChanged)
            Q_CLASSINFO("curNo", "display=工位号;edit_type=ET_SpinBox;readonly=true;min=1;max=10000000")           

            Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
            Q_CLASSINFO("name", "display=工位名称;edit_type=ET_Edit")

            Q_PROPERTY(QString desc READ desc WRITE setDesc NOTIFY descChanged)
            Q_CLASSINFO("desc", "display=工位描述;edit_type=ET_Edit")

            Q_PROPERTY(WorkProcedure procedure READ procedure WRITE setProcedure NOTIFY procedureChanged)
            Q_CLASSINFO("procedure", "display=工位流程;edit_type=ET_ComboBox;enums=检测流程，标定流程，定位流程")

            Q_PROPERTY(bool isSend READ isSend WRITE setIsSend NOTIFY isSendChanged)
            Q_CLASSINFO("isSend", "display=是否发送结果;edit_type=ET_CheckBox")

            Q_PROPERTY(bool isHardTrigger READ isHardTrigger WRITE setIsHardTrigger NOTIFY isHardTriggerChanged)
            Q_CLASSINFO("isHardTrigger", "display=是否硬触发;edit_type=ET_CheckBox")          

            Q_PROPERTY(CommType commType READ commType WRITE setCommType NOTIFY commTypeChanged)
            Q_CLASSINFO("commType", "display=命令类型;edit_type=ET_ComboBox;enums=串口，TCP，PLC")

            Q_PROPERTY(DisPlayResultPos displayResultPos READ displayResultPos WRITE setDisplayResultPos NOTIFY displayResultPosChanged)
            Q_CLASSINFO("displayResultPos", "display=窗口类型;edit_type=ET_ComboBox;enums=单窗格，四窗格，九窗格，十六窗格，自定义")
            
            Q_PROPERTY(QString displayTitle READ displayTitle WRITE setDisplayTitle NOTIFY displayTitleChanged)
            Q_CLASSINFO("displayTitle", "display=窗格名称;edit_type=ET_Edit")

    public:
        explicit VisionWorkStation(QObject* parent = nullptr);

        bool isSend() const;
        void setIsSend(bool newIsSend);

        bool isHardTrigger() const;
        void setIsHardTrigger(bool newIsHardTrigger);

        int curNo() const;
        void setCurNo(int newCurNo);

        WorkProcedure procedure() const;
        void setProcedure(WorkProcedure newProcedure);

        QString name() const;
        void setName(const QString& newName);

        QString desc() const;
        void setDesc(const QString& newDesc);

        CommType commType() const;
        void setCommType(CommType newCommType);

        DisPlayResultPos displayResultPos() const;
        void setDisplayResultPos(DisPlayResultPos pos);

        QString displayTitle() const;
        void setDisplayTitle(QString title);

        QJsonObject toJson() const;
        void fromJson(const QJsonObject& json);

    signals:
        void isSendChanged();
        void isHardTriggerChanged();
        void curNoChanged();
        void procedureChanged();
        void nameChanged();
        void descChanged();
        void commTypeChanged();
        void displayResultPosChanged();
        void displayTitleChanged();

    private:
        bool is_send_ = true;
        bool is_hard_trigger_ = false;
        int cur_no_ = 0;
        WorkProcedure procedure_;
        QString name_;
        QString desc_;
        CommType comm_type_;
        DisPlayResultPos display_result_pos_;
        QString display_title_;
    };
}
#endif // !VISION_WORK_STATION_H