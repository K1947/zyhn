#ifndef SELECTABLE_WIDGET_H
#define SELECTABLE_WIDGET_H

#include <QWidget>
#include <QMouseEvent>

class SelectableWidget : public QWidget
{
    Q_OBJECT
    
        Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
public:
    explicit SelectableWidget(QWidget* parent = nullptr);

    bool isChecked() const;
    void setChecked(bool checked);

    void setGroupName(const QString& groupName);
    QString groupName() const;

    static void addToGroup(SelectableWidget* widget, const QString& groupName = "default");
    static void setExclusiveGroup(const QString& groupName, SelectableWidget* checkedWidget);

signals:
    void checkedChanged(bool checked);
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
	void paintEvent(QPaintEvent* event) override;

private:
    bool m_checked;
    QString m_groupName;
    bool m_pressed;

    static QHash<QString, QList<SelectableWidget*>> m_groupWidgets;
};

#endif // !SELECT_TABLE_WIDGET_H