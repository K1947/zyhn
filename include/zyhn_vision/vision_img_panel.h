#ifndef VISION_IMG_PANEL_H
#define VISION_IMG_PANEL_H
#include <QWidget>
#include "zyhn_vision_global.h"
#include <QButtonGroup>
#include "zyhn_core.h"

namespace Ui {
    class vision_img_panel;
}

class ZYHN_VISION_EXPORT VisionImgPanel: public QWidget
{
    Q_OBJECT
public:
    VisionImgPanel(zyhn_core::IApplication* app, QWidget * parent = nullptr);
    ~VisionImgPanel();
    int customImageCount() const;
private slots:
    void on_btn_save_count_clicked();
    void onBtnGroupClicked(int idx);
    void onProductChanged();
signals:
    void layoutChanged(int count);
private:
    Ui::vision_img_panel* ui_;
    QButtonGroup* btn_groups_;
    zyhn_core::IApplication* app_;
    int cur_cusotm_count_;
};

#endif