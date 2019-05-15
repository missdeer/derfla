#ifndef DERFLAAPP_H
#define DERFLAAPP_H

#include <QObject>
#include <QSettings>

QT_FORWARD_DECLARE_CLASS(QSystemTrayIcon)

class ExtensionManager;
class DerflaWidget;
class AlfredWidget;
class AutoUpdater;

class DerflaApp : public QObject
{
    Q_OBJECT
    
    friend class DerflaWidget;
    friend class AlfredWidget;
public:
    explicit DerflaApp(QObject *parent = nullptr);
    ~DerflaApp() override;
    
    void createDerflaWidget();
    void showDerflaWidget();
    void createAlfredWidget();
    void showAlfredWidget();
    
    void autoUpdate();
    void checkForUpdates();
signals:
    
public slots:
    
private:
    ExtensionManager *extensionManager_;
    QSystemTrayIcon* trayIcon_;
    DerflaWidget* derflaWidget_ = nullptr;
    AlfredWidget* alfredWidget_ = nullptr;
    AutoUpdater* autoUpdater_ = nullptr;
    QSettings settings_;    
    void CenterToScreen(QWidget* widget);    
};

inline DerflaApp *derflaApp = nullptr;

#endif // DERFLAAPP_H
