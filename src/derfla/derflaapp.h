#ifndef DERFLAAPP_H
#define DERFLAAPP_H

#include <QObject>

QT_FORWARD_DECLARE_CLASS(QSystemTrayIcon)

class ExtensionManager;
class DerflaWidget;
class AlfredWidget;

class DerflaApp : public QObject
{
    Q_OBJECT
    
    friend class DerflaWidget;
    friend class AlfredWidget;
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
    AlfredWidget* alfredWidget_;
};

#endif // DERFLAAPP_H
