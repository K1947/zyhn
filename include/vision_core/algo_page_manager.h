#ifndef ALGO_PAGE_MANAGER_H
#define ALGO_PAGE_MANAGER_H

#include <QObject>
#include <QStackedWidget>
#include <QMap>
#include <QVariant>
#include "page_base.h"
#include "vision_core_global.h"

class VISION_CORE_EXPORT AlgoPageManager : public QObject
{
    Q_OBJECT
public:
    explicit AlgoPageManager(QStackedWidget* stackedWidget, QObject* parent = nullptr);
    void addPage(PageBase* page);
    void removePage(const QString& pageId);
    void switchToPage(const QString& pageId, const QVariant& data = QVariant());
    PageBase* currentPage() const;
    PageBase* getPage(const QString& pageId) const;
    void setSharedData(const QString& key, const QVariant& value);
    QVariant getSharedData(const QString& key) const;
    void broadcastMessage(const QString& message, const QVariant& data = QVariant());
    QStringList getPageIdsByName(const QString& pageName) const;
    QImage getPageImageById(QString pageId) const;
    void setModelImgStoragePath(const QString&);
    QString modelImgStoragePath() const;
    QString modelImageName(const QString& pageid);
signals:
    void pageChanged(const QString& fromPage, const QString& toPage);
    void dataChanged(const QString& key, const QVariant& value);
    void messageReceived(const QString& message, const QVariant& data);
    void pageAdded(const QString& pageName);
    void pageRemoved(const QString& pageName);

private slots:
    void onStackedWidgetCurrentChanged(int index);

private:
    QStackedWidget* stacked_widget_;
    QMap<QString, PageBase*> pages_;
    QMap<QString, QVariant> shared_data_;
    QString current_page_id_;
    QString image_save_path_;
};

#endif // PAGEMANAGER_H