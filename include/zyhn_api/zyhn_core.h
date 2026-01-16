#ifndef ZYHN_CORE_H
#define ZYHN_CORE_H

#include <QObject>
#include <QWidget>
#include <QMainWindow>
#include <QSettings>
#include "zyhn_obj.h"
#include "zyhn_user.h"
#include "camera.h"
#include "zyhn_api_global.h"

namespace zyhn_core {

    class IApplication;

    class ZYHN_API_EXPORT IManager : public QObject
    {
        Q_OBJECT
    public:
        IManager(QObject* parent = nullptr) : QObject(parent) {}
        virtual ~IManager() = default;
        virtual bool initWithApp(IApplication* app) {
            zyhn_app_ = app;
            return true;
        }
        virtual IApplication* application() {
            return zyhn_app_;
        }

    protected:
        IApplication* zyhn_app_;
    };

    class IPlugin;
    class IProductManager;
    class IUserManager;
    class IAppDatabase;
    class IPluginManager;
    class ICameraManager;
    class IAlgoDataManager;
    class ZYHN_API_EXPORT IApplication : public IObject
    {
        Q_OBJECT
    public:
        virtual ~IApplication() {}

        virtual QString applicationPath() const = 0;
        virtual QString pluginPath() const = 0;
        virtual QString storagePath() const = 0;
        virtual QString algorithmPath() const = 0;

        virtual QMainWindow* mainWindow() const = 0;

        virtual QSettings* settings() = 0;

        virtual QList<IPlugin*> pluginList() const = 0;
        virtual IPlugin* findPluginById(const QString& id) const = 0;
        virtual IProductManager* productManager() const = 0;
        virtual IUserManager* userManager() const = 0;
        virtual IAppDatabase* appDb() const = 0;
        virtual IPluginManager* pluginManager() const = 0;
        virtual ICameraManager* cameraManager() const = 0;
        virtual IAlgoDataManager* algoDataManager() const = 0;
    signals:
        void Loaded();
        void loadPluginsComplete();
    };

    class ZYHN_API_EXPORT IPlugin : public IObject
    {
        Q_OBJECT
    public:
        virtual ~IPlugin() = default;
        virtual bool load(IApplication* app) = 0;
        virtual QWidget* createWgt(IApplication* app, QString wgtName = "", QWidget* parent = nullptr) {
            Q_UNUSED(app);
            Q_UNUSED(wgtName);
            Q_UNUSED(parent);
            return nullptr;
        }
        virtual QDialog* createDlg(IApplication* app, QString wgtName = "", QWidget* parent = nullptr) {
            Q_UNUSED(app);
            Q_UNUSED(wgtName);
            Q_UNUSED(parent);
            return nullptr;
        }
    };

    class ZYHN_API_EXPORT PluginInfo
    {
    public:
        PluginInfo() : mustLoad_(true) {}
        virtual ~PluginInfo() {}
        QString author() const { return author_; }
        QString info() const { return info_; }
        QString id() const { return id_; }
        QString name() const { return name_; }
        QString ver() const { return ver_; }
        QStringList dependList() const { return dependList_; }
        QString filePath() const { return filePath_; }
        bool isMustLoad() const { return mustLoad_; }
        void setAuthor(const QString& author) { author_ = author; }
        void setInfo(const QString& info) { info_ = info; }
        void setId(const QString& id) { id_ = id.toLower(); }
        void setName(const QString& name) { name_ = name; }
        void setVer(const QString& ver) { ver_ = ver; }
        void setFilePath(const QString& path) { filePath_ = path; }
        void setDependList(const QStringList& dependList) { dependList_ = dependList; }
        void appendDepend(const QString& depend) { dependList_.append(depend); }
        void setMustLoad(bool b) { mustLoad_ = b; }
    protected:
        bool mustLoad_;
        QString author_;
        QString info_;
        QString id_;
        QString name_;
        QString filePath_;
        QString ver_;
        QStringList dependList_;
    };

    class ZYHN_API_EXPORT IPluginFactory : public QObject
    {
        Q_OBJECT
    public:
        virtual ~IPluginFactory() {}
        virtual QString id() const = 0;
        virtual PluginInfo* info() const = 0;
        virtual QStringList dependPluginList() const = 0;
        virtual void setFilePath(const QString& path) = 0;
        virtual QString filePath() const = 0;
        virtual IPlugin* createPlugin() = 0;
    };

    class ZYHN_API_EXPORT IPluginFactoryImpl : public IPluginFactory
    {
        Q_OBJECT
    public:
        IPluginFactoryImpl() : info_(new PluginInfo)
        {
        }
        virtual ~IPluginFactoryImpl()
        {
            delete info_;
        }
        virtual QString id() const
        {
            return info_->id();
        }
        virtual PluginInfo* info() const
        {
            return info_;
        }
        virtual QStringList dependPluginList() const {
            return info_->dependList();
        }
        virtual void setFilePath(const QString& path)
        {
            info_->setFilePath(path);
        }
        virtual QString filePath() const
        {
            return info_->filePath();
        }
    protected:
        PluginInfo* info_;
    };

    template <typename T>
    class PluginFactoryT : public IPluginFactoryImpl
    {
    public:
        virtual IPlugin* createPlugin()
        {
            return new T;
        }
    };

    class ZYHN_API_EXPORT IUserManager : public IManager
    {
        Q_OBJECT
    public:
        IUserManager(QObject* parent = nullptr) : IManager(parent) {}
        ~IUserManager() override = default;

        virtual bool login(const QString& username, const QString& password) = 0;
        virtual void logout() = 0;
        virtual bool isLoggedIn() const = 0;
        virtual UserInfo getCurrentUser() const = 0;
        virtual QVector<UserInfo> userList() const = 0;
        virtual void refreshUserList() = 0;
        virtual bool addUser(const UserInfo& user) = 0;
        virtual bool updateUser(const UserInfo& user) = 0;
        virtual bool removeUser(int userId) = 0;
        virtual UserInfo findUserById(int userId) const = 0;
        virtual UserInfo findUserByUsername(const QString& username) const = 0;
        virtual bool hasPermission(Permission permission) const = 0;
        virtual bool canManageUsers() const = 0;
    signals:
        void userLoggedIn(int uid);
        void userLoggedOut();
        void userAdded(int uid);
        void userUpdated(int uid);
        void userDeleted(int uid);
        void permissionChanged();
    };

    class ZYHN_API_EXPORT IProductManager : public IManager
    {
        Q_OBJECT
    public:
        IProductManager(QObject* parent = nullptr) : IManager(parent) {}
        ~IProductManager() override = default;
        virtual QJsonArray productList() const = 0;
        virtual bool setCurrentProduct(const int pid) = 0;
        virtual QJsonObject currentProduct() const = 0;
        virtual QJsonObject findProductById(const int pid) const = 0;
        virtual QJsonObject findDelProductById(const int pid) const = 0;
        virtual QJsonObject findProductByName(const QString name) const = 0;
        virtual bool addProduct(const QJsonObject& obj) = 0;
        virtual bool updateProduct(const QJsonObject& obj) = 0;
        virtual bool removeProduct(const int pid) = 0;
        virtual void openProductListWnd() = 0;
        virtual void refreshProductList() = 0;
        virtual int getNextProductId() = 0;
    signals:
        void productListChanged(); // 产品列表变化
        void productSwitched(); // 当前产品切换信号
        void productAdded(int); // 当前产品增加
        void productDeled(int); // 当前产品删除
    };

    class ZYHN_API_EXPORT ICameraManager : public IManager
    {
        Q_OBJECT
    public:
        ICameraManager(QObject* parent = nullptr) : IManager(parent) {};
        ~ICameraManager() override = default;

        virtual void initialize() = 0;
        virtual QStringList cameraNameList() = 0;
        virtual Camera* camera(const QString& cameraname) = 0;
        virtual bool addCamera(Camera* camera) = 0;
        virtual QList<Camera*> getCameras() = 0;
    };

    class ZYHN_API_EXPORT IAlgoDataManager : public IManager
    {
        Q_OBJECT
    public:
        IAlgoDataManager(QObject* parent = nullptr) : IManager(parent) {};
        ~IAlgoDataManager() override = default;

        virtual bool saveAlgoParam(const QJsonArray& info, int id) = 0;
        virtual bool updateAlgo(const QJsonObject& info, int id) = 0;
        virtual bool deleteAlgo(int id) = 0;
        virtual QJsonArray getAlgoParam(int id) = 0;
    };

    class ZYHN_API_EXPORT IPluginManager : public IManager
    {
        Q_OBJECT
    public:
        IPluginManager(QObject* parent = nullptr) : IManager(parent) {}
        ~IPluginManager() override = default;

        virtual void loadPlugins(const QString& dir) = 0;
        virtual QList<IPluginFactory*> getPluginList() const = 0;
    };

    class ZYHN_API_EXPORT IAppDatabase : public QObject
    {
        Q_OBJECT
        public:
            IAppDatabase(QObject* parent = nullptr) : QObject(parent) {}
        virtual ~IAppDatabase() = default;
        virtual bool initialize(const QString& dbPath = "") = 0;
        virtual void close() = 0;
        virtual int getDatabaseVersion() = 0;

        virtual bool saveData(const QString& tableName, int id, const QString& jsonData,
            const QMap<QString, QVariant>& indexFields = QMap<QString, QVariant>()) = 0;
        virtual bool deleteData(const QString& tableName, int id) = 0;

        virtual QVector<QString> loadAllData(const QString& tableName) = 0;
        virtual QString findDataById(const QString& tableName, int id) = 0;
        virtual QVector<QString> queryData(const QString& tableName, const QString& whereClause) = 0;
        virtual int getNextId(const QString& tableName) = 0;
        virtual bool createTable(const QString& tableName, const QStringList& indexFields = QStringList()) = 0;
        virtual bool tableExists(const QString& tableName) = 0;
        virtual QStringList getAllTables() = 0;
        virtual bool beginTransaction() = 0;
        virtual bool commitTransaction() = 0;
        virtual bool rollbackTransaction() = 0;
        virtual bool clearTable(const QString& tableName) = 0;
        virtual bool dropTable(const QString& tableName) = 0;
        virtual bool compactDatabase() = 0;
        virtual QMap<QString, int> getDatabaseStats() = 0;
    };
}

Q_DECLARE_INTERFACE(zyhn_core::IPluginFactory, "zyhn.core.IPluginFactory.X1")

#endif // !ZYHN_CORE_H