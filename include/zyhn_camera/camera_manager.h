#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H

#include "zyhn_camera_global.h"
#include "zyhn_core.h"

using namespace zyhn_core;

class CameraDriverHik;
class CameraDriverVirtual;

class ZYHN_CAMERA_EXPORT CameraManager: public ICameraManager
{
    Q_OBJECT
public:
    CameraManager(QObject* parent = nullptr);
    ~CameraManager();
    virtual void initialize() override;
    virtual QStringList cameraNameList() override;
    virtual Camera* camera(const QString& cameraname) override;
    virtual bool addCamera(Camera* driver) override;
    virtual QList<Camera*> getCameras() override;
private:
    QMap<QString, Camera*> camera_map_;
    CameraDriverHik* camera_hik_driver_;
    CameraDriverVirtual* camera_virtual_driver_;
};

#endif // !CAMERA_MANAGER_H