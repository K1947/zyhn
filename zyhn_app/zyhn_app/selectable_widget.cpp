#include "selectable_widget.h"
#include <QStyle>
#include <QDebug>
#include <QStyleOption>
#include <QPainter>
#include <Qlabel>

QHash<QString, QList<SelectableWidget*>> SelectableWidget::m_groupWidgets;
SelectableWidget::SelectableWidget(QWidget* parent)
    : QWidget(parent)
    , m_checked(false)
    , m_pressed(false)
{
    setMouseTracking(true);
}

bool SelectableWidget::isChecked() const
{
    return m_checked;
}

void SelectableWidget::setChecked(bool checked)
{
    if (m_checked == checked)
        return;

    m_checked = checked;
    style()->unpolish(this);
    style()->polish(this);
    QList<QLabel*> children = this->findChildren<QLabel*>();
    for (QLabel* label : children) {
        label->style()->unpolish(label);
        label->style()->polish(label);
	}
    update();

    emit checkedChanged(m_checked);
}

void SelectableWidget::setGroupName(const QString& groupName)
{
    m_groupName = groupName;
}

QString SelectableWidget::groupName() const
{
    return m_groupName;
}

void SelectableWidget::addToGroup(SelectableWidget* widget, const QString& groupName)
{
    if (!m_groupWidgets.contains(groupName)) {
        m_groupWidgets[groupName] = QList<SelectableWidget*>();
    }

    if (!m_groupWidgets[groupName].contains(widget)) {
        m_groupWidgets[groupName].append(widget);
        widget->setGroupName(groupName);
    }
}

void SelectableWidget::setExclusiveGroup(const QString& groupName, SelectableWidget* checkedWidget)
{
    if (!m_groupWidgets.contains(groupName))
        return;

    for (SelectableWidget* widget : m_groupWidgets[groupName]) {
        if (widget != checkedWidget) {
            widget->setChecked(false);
        }
    }

    if (checkedWidget && !checkedWidget->isChecked()) {
        checkedWidget->setChecked(true);
    }
}

void SelectableWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void SelectableWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_pressed) {
        m_pressed = false;

        if (rect().contains(event->pos())) {
            if (!m_groupName.isEmpty()) {
                setExclusiveGroup(m_groupName, this);
            }
            else {
                setChecked(true);
            }

            emit clicked();
            event->accept();
            return;
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void SelectableWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QStyleOption styleOpt;
    styleOpt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &styleOpt, &painter, this);
}