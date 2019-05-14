#ifndef DERFLAAPP_H
#define DERFLAAPP_H

#include <QObject>

QT_FORWARD_DECLARE_CLASS(QSystemTrayIcon)

class ExtensionManager;
class DerflaWidget;

class DerflaApp : public QObject
{
    Q_OBJECT
public:
    explicit DerflaApp(QObject *parent = nullptr);
    
    void createDerflaWidget();
    void showDerflaWidget();
    void createAlfredWidget();
    void showAlfredWidget();
signals:
    
public slots:
    
private:
    ExtensionManager *extensionManager_;
    QSystemTrayIcon* trayIcon_;
    DerflaWidget* derflaWidget_;
};

#endif // DERFLAAPP_H
