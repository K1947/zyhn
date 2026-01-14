#include "zyhn_app.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include "permission_utils.h"
#include "log4qt/propertyconfigurator.h"
#include "loading_manager.h"
#include "algorithm_manager.h"

ZyhnApp::ZyhnApp()
{
    Log4Qt::PropertyConfigurator::configure(applicationPath() + "/zyhn.log.config");
    app_database_ = new AppDatabase(this);
    product_manager_ = new zyhn_product::ProductManager(this);
    plugin_manager_ = new PluginManager(this);
    user_manager_ = new UserManager(this);
    camera_manager_ = new CameraManager(this);
    algo_data_manager_ = new AlgoDataManager(this);
    main_wnd_ = new ZyhnMainWindow(this);

    LoadingManager::instance().setParentWidget(main_wnd_);
    LoadingManager::instance().setGifPath(":/zyhn_app/style/img/loading.gif");

    PermissionUtils::setUserManager(user_manager_);
}

ZyhnApp::~ZyhnApp()
{
    delete camera_manager_;
}

QString ZyhnApp::applicationPath() const
{
    return qApp->applicationDirPath();
}

QString ZyhnApp::pluginPath() const
{
    return applicationPath(); // +"/plugins";
}

QString ZyhnApp::storagePath() const
{
    return applicationPath() + "/storage";
}

QString ZyhnApp::algorithmPath() const
{
    return applicationPath() + "/algorithm";
}

void ZyhnApp::load()
{
    // 确保storage目录存在
    QDir storageDir(storagePath());
    if (!storageDir.exists()) {
        storageDir.mkpath(".");
        qDebug() << "Created storage directory:" << storagePath();
    }

    pluginManager()->loadPlugins(pluginPath());
    initPlugins();
    app_database_->initialize(storagePath());
    product_manager_->initWithApp(this);
    user_manager_->initWithApp(this);
    algo_data_manager_->initWithApp(this);
    user_manager_->refreshUserList();
    user_manager_->setLoginUser();
    product_manager_->refreshProductList();
    product_manager_->setCurrentProduct(-1);
    emit product_manager_->productListChanged();
    camera_manager_->initialize();
    AlgorithmManager::instance().loadPlugins(algorithmPath());
    if (main_wnd_) {
        main_wnd_->initPluginWidget();
        main_wnd_->showMaximized();
    }

    emit Loaded();
}

QMainWindow* ZyhnApp::mainWindow() const
{
    return main_wnd_;
}

QSettings* ZyhnApp::settings()
{
    if (!settings_) {
        QString settingsFile = storagePath() + "/zyhncfg.ini";
        settings_ = new QSettings(settingsFile, QSettings::IniFormat);
    }
    return settings_;
}

IPluginManager* ZyhnApp::pluginManager() const
{
    return plugin_manager_;
}

QList<IPlugin*> ZyhnApp::pluginList() const
{
    return plugin_map_.values();
}

IProductManager* ZyhnApp::productManager() const
{
    return product_manager_;
}

IUserManager* ZyhnApp::userManager() const
{
    return user_manager_;
}

ICameraManager* ZyhnApp::cameraManager() const
{
    return camera_manager_;
}

IAlgoDataManager* ZyhnApp::algoDataManager() const
{
    return algo_data_manager_;
}

IAppDatabase* ZyhnApp::appDb() const
{
    return app_database_;
}

void ZyhnApp::initPlugins()
{
    IPluginManager* manager = pluginManager();
    manager->loadPlugins(pluginPath());
    auto factories = manager->getPluginList();
    for (IPluginFactory* factory : factories) {
        bool isLoad = settings()->value(QString("plugin\\%1_load").arg(factory->id()), true).toBool();
        if (!isLoad) {
            continue;
        }

        IPlugin* plugin = factory->createPlugin();
        if (plugin->load(this)) {
            plugin_map_.insert(factory->id(), plugin);
        } else {
            delete plugin;
        }
    }
}

IPlugin* ZyhnApp::findPluginById(const QString& id) const
{
    return plugin_map_.value(id, nullptr);
}