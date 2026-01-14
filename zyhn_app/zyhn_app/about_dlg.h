#ifndef ABOUT_DLG_H
#define ABOUT_DLG_H

#include "basedlg.h"
#include "ui_about_dlg.h"

class AboutDlg: public BaseDlg
{
    Q_OBJECT
public:
    AboutDlg(QWidget* parent = nullptr);
    ~AboutDlg();
private:
    QString getWindowsArchitecture();
    QString getWindowsOSVersion();
    QString fileVersion(const QString& filePath);
    QString getMacAddress();
    QString getIpAddress();
private:
    Ui::about_dlg ui_;
};

#endif // !ABOUT_DLG_H