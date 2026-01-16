#ifndef ZYHN_VISION_WIDGET_H 
#define ZYHN_VISION_WIDGET_H

#include "zyhn_vision_global.h"
#include "zyhn_core.h"
#include <QWidget>
#include <QButtonGroup>
#include <QScrollArea>

namespace Ui {
    class vision_widget;
}

using namespace zyhn_core;
class ZYHN_VISION_EXPORT ZyhnVisionWidget: public QWidget
{
    Q_OBJECT
public:
    ZyhnVisionWidget(IApplication* app, QWidget* parent = nullptr);
    ~ZyhnVisionWidget();
    void setBtnGroup();
    void setWorkStation(); // 设置工位信息
private:
    void initBtnGroup();
    void createNewPage(int index);
private slots:
    void btnGroupClicked(int id);
    void productListChangedSlots();
private:
    Ui::vision_widget* ui_;
    IApplication* app_;
    QButtonGroup* btn_group_;
    int btn_id_;
};

#endif // !ZYHN_VISION_WIDGET_H 