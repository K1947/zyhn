#ifndef PRODUCT_MANAGER_H
#define PRODUCT_MANAGER_H
#include <QDialog>
#include "zyhn_core.h"
#include "product_info.h"
#include "zyhn_product_global.h"

using namespace zyhn_core;
namespace zyhn_product {
    class ProductDataManager;
    class ZYHN_PRODUCT_EXPORT ProductManager : public IProductManager
    {
        Q_OBJECT
    public:
        ProductManager(QObject* parent = nullptr);
        ~ProductManager() override = default;
        bool initWithApp(IApplication* app);
        QJsonArray productList() const override;
        bool setCurrentProduct(const int pid) override;
        QJsonObject currentProduct() const override;
        QJsonObject findProductById(const int pid) const override;
        QJsonObject findDelProductById(const int pid) const override;
        QJsonObject findProductByName(const QString name) const override;
        bool addProduct(const QJsonObject& info) override;
        bool updateProduct(const QJsonObject& info) override;
        bool removeProduct(const int pid) override;
        void openProductListWnd() override;
        void refreshProductList() override;
        int getNextProductId() override;
    private:
        int current_product_id_;
        QVector<ProductInfo*> product_list_;
        ProductDataManager* pdb_;
    };
}
#endif // !PRODUCT_MANAGER_H


