#ifndef ZYHN_APP_H
#define ZYHN_APP_H
#include "zyhn_core.h"
#include "zyhn_main_window.h"
#include "product_manager.h"
#include "plugin_manager.h"
#include "app_database.h"
#include "user_manager.h"
#include "camera_manager.h"
#include "algo_data_manager.h"

using namespace zyhn_core;
class ZyhnApp : public zyhn_core::IApplication
{
    Q_OBJECT
public:
    ZyhnApp();
    virtual ~ZyhnApp();
    virtual IPluginManager* pluginManager() const override;
    virtual QString applicationPath() const override;
    virtual QString pluginPath() const override;
    virtual QString storagePath() const override;
    virtual QString algorithmPath() const override;
    virtual QList<IPlugin*> pluginList() const override;
    virtual IPlugin* findPluginById(const QString& id) const override;
    virtual QMainWindow* mainWindow() const override;
    virtual QSettings* settings() override;
    virtual IProductManager* productManager() const override;
    virtual IAppDatabase* appDb() const override;
    virtual IUserManager* userManager() const override;
    virtual ICameraManager* cameraManager() const override;
    virtual IAlgoDataManager* algoDataManager() const override;
    void load();
private:
    void initPlugins();
private:
    ZyhnMainWindow* main_wnd_ = nullptr;
    QSettings* settings_ = nullptr;
    zyhn_product::ProductManager* product_manager_ = nullptr;
    QMap<QString, IPlugin*> plugin_map_;
    AppDatabase* app_database_;
    UserManager* user_manager_ = nullptr;
    PluginManager* plugin_manager_ = nullptr;
    CameraManager* camera_manager_ = nullptr;
    AlgoDataManager* algo_data_manager_ = nullptr;
};

#endif // !ZYHN_APP_H