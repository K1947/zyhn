#ifndef PRODUCT_LIST_DLG_H
#define PRODUCT_LIST_DLG_H

#include "basedlg.h"
#include "zyhn_core.h"
#include "zyhn_product_global.h"

namespace Ui {
    class product_list_dlg;
}
namespace zyhn_product {

    class ZYHN_PRODUCT_EXPORT ProductListDlg : public BaseDlg
    {
        Q_OBJECT
    public:
        explicit ProductListDlg(zyhn_core::IApplication* app, QWidget* parent = nullptr);
        ~ProductListDlg() override;
    private:
        void initTableWidget();
        void setProductData();
        QWidget* createTableBtnWidget(int pid,
            const QString& icon1,
            const QString& icon2,
            QObject* receiver);
    private slots:
        void on_btn_add_product_clicked();
    private:
        Ui::product_list_dlg *ui_;
        zyhn_core::IApplication* app_;
    };
}

#endif // !PRODUCT_LIST_DLG_H
