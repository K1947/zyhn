#ifndef BASE_DLG_H
#define BASE_DLG_H

#include "basedlg_global.h"
#include <QDialog>
#include <QLabel>
#include <QSizeGrip>
#include <QPushButton>

class BASEDLG_EXPORT BaseDlg : public QDialog
{
    Q_OBJECT
public:
    explicit BaseDlg(QWidget* parent = nullptr);
    QWidget* ContentArea();
    void setResizable(bool resizable);
    void setTitleText(const QString& text);
    void setIcon(const QPixmap& icon);
    void setMinBtnVisible(bool isVisible = false);
    void setMaxBtnVisible(bool isVisible = false);
    void setCloseHandler(std::function<void()> handler);

private:
    void initTitleBar();
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

protected:
    QWidget* title_bar_;
    QLabel* icon_label_;
    QLabel* text_label_;
    QPushButton* min_btn_;
    QPushButton* max_btn_;
    QPushButton* close_btn_;
    QWidget* content_area_;
    QSizeGrip* size_grip_;

    bool dragging_ = false;
    QPoint drag_start_pos_;
    std::function<void()> close_handler_;
};

#endif // !BASE_DLG_H