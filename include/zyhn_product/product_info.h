#ifndef PRODUCT_H
#define PRODUCT_H
#include <QObject>
#include "vision_work_station.h"
#include "control_work_station.h"
#include "product_def.h"
#include <QJsonObject>
#include "zyhn_product_global.h"

namespace zyhn_product {    

    class ZYHN_PRODUCT_EXPORT ProductInfo : public QObject {
        Q_OBJECT

            Q_PROPERTY(int startAddress READ startAddress WRITE setStartAddress NOTIFY startAddressChanged)
            Q_CLASSINFO("startAddress", "display=开始地址;edit_type=ET_SpinBox")

            Q_PROPERTY(int endAddress READ endAddress WRITE setEndAddress NOTIFY endAddressChanged)
            Q_CLASSINFO("endAddress", "display=结束地址;edit_type=ET_SpinBox")

            Q_PROPERTY(TriggerType triggerType READ triggerType WRITE setTriggerType NOTIFY triggerTypeChanged)
            Q_CLASSINFO("triggerType", "display=触发方式;edit_type=ET_ComboBox;enums=软触发，硬触发")

            Q_PROPERTY(QString saveImagePath READ saveImagePath WRITE setSaveImagePath NOTIFY saveImagePathChanged)
            Q_CLASSINFO("saveImagePath", "display=产品图片路径;edit_type=ET_Edit")

            Q_PROPERTY(SaveImageType saveImageType READ saveImageType WRITE setSaveImageType NOTIFY saveImageTypeChanged)
            Q_CLASSINFO("saveImageType", "display=保存图片类型;edit_type=ET_ComboBox;enums=原始图片，压缩图片")

            Q_PROPERTY(bool isSaveImage READ isSaveImage WRITE setIsSaveImage NOTIFY isSaveImageChanged)
            Q_CLASSINFO("isSaveImage", "display=是否保存图片;edit_type=ET_CheckBox")

            Q_PROPERTY(QString defects READ defects WRITE setDefects NOTIFY defectsChanged)
            Q_CLASSINFO("defects", "display=缺陷类别;edit_type=ET_Edit")

    public:
        explicit ProductInfo(QObject* parent = nullptr);
        Q_INVOKABLE QList<std::shared_ptr<QObject>> visionWorkStationsAsObjects() const;
        Q_INVOKABLE QList<std::shared_ptr<QObject>> controlWorkStationsAsObjects() const;

        int id() const;
        void setId(int newId);

        QString name() const;
        void setName(const QString& newName);

        QString desc() const;
        void setDesc(const QString& newDesc);

        QString author() const;
        void setAuthor(const QString& newAuthor);

        QString createDate() const;
        void setCreateDate(const QString& newCreateDate);

        int startAddress() const;
        void setStartAddress(int newStartAddress);

        int endAddress() const;
        void setEndAddress(int newEndAddress);

        TriggerType triggerType() const;
        void setTriggerType(TriggerType newTriggerType);

        QString saveImagePath() const;
        void setSaveImagePath(const QString& newSaveImagePath);

        SaveImageType saveImageType() const;
        void setSaveImageType(SaveImageType newSaveImageType);

        bool isSaveImage() const;
        void setIsSaveImage(bool newIsSaveImage);

        QString defects() const;
        void setDefects(const QString& newDefects);

        QList<std::shared_ptr<VisionWorkStation>> visionWorkStations() const;
        void addVisionWorkStation(std::shared_ptr<VisionWorkStation> station);
        std::shared_ptr<VisionWorkStation> createVisionWorkStation();

        QList<std::shared_ptr<ControlWorkStation>> controlWorkStations() const;
        void addControlWorkStation(std::shared_ptr<ControlWorkStation> station);
        std::shared_ptr<ControlWorkStation> createControlWorkStation();

        QJsonObject toJson() const;
        void fromJson(const QJsonObject& json);

        int copyId() const;
        void setCopyId(int id);

    signals:
        void idChanged();
        void nameChanged();
        void descChanged();
        void authorChanged();
        void createDateChanged();
        void startAddressChanged();
        void endAddressChanged();
        void triggerTypeChanged();
        void saveImagePathChanged();
        void saveImageTypeChanged();
        void isSaveImageChanged();
        void defectsChanged();

    private:
        int id_ = -1;
        QString name_;
        QString desc_;
        QString author_;
        QString create_date_;
        int start_address_ = 0;
        int end_address_ = 0;
        TriggerType trigger_type_;
        QString save_image_path_;
        SaveImageType save_image_type_;
        bool is_save_image_ = true;
        QString defects_;
        bool copy_pid_;

        // 工位列表（父子管理）
        QList<std::shared_ptr<VisionWorkStation>> vision_stations_;
        QList<std::shared_ptr<ControlWorkStation>> control_stations_;
    };

}
#endif // !PRODUCT_H