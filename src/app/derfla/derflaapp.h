#ifndef DERFLAAPP_H
#define DERFLAAPP_H

#include <QSettings>
#include <QSystemTrayIcon>

#include "actionexecutor.h"

QT_FORWARD_DECLARE_CLASS(QAction)

class ExtensionManager;
class DerflaWidget;
class AlfredWidget;
class CommonWidget;

class DerflaApp : public QObject
{
    Q_OBJECT

    friend class DerflaWidget;
    friend class AlfredWidget;

public:
    explicit DerflaApp(QObject *parent = nullptr);
    ~DerflaApp() override;

    void createWidget();
    void show();

    DerflaWidget *createDerflaWidget();
    void          showDerflaWidget();
    AlfredWidget *createAlfredWidget();
    void          showAlfredWidget();

    void              executeAction(const DerflaActionPtr &action);
    ExtensionManager *extensionManager();
    void              queryByExtension(const QString &text);
    void              setCheckedStayOnTopAction(bool checked);
    QSettings        &settings();

    [[nodiscard]] bool              isEmptyDerflaAction() const;
    DerflaActionPtr                 derflaAction(int index);
    [[nodiscard]] qsizetype         derflaActionCount() const;
    void                            clearDerflaAction();
    [[nodiscard]] DerflaActionList &derflaActions();
    [[nodiscard]] DerflaActionList &donateDerflaActions();
signals:
    void actionUpdated();
    void emptyAction();
public slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onLoadSkin();
    void onStayOnTop();
    void onShowInFront();
    void onQuit();
    void onSelectFile();
    void onSelectFolder();
    void onAbout();
    void onPreference();
    void donateViaPaypal();
    void donateViaAlipay();
    void donateViaWeChatPay();
private slots:
    void onActionUpdated(DerflaActionList &dal);
    void onEmptyAction();

private:
    ExtensionManager *extensionManager_;
    QSystemTrayIcon  *trayIcon_;
    CommonWidget     *widget_ {nullptr};
    DerflaWidget     *derflaWidget_ {nullptr};
    AlfredWidget     *alfredWidget_ {nullptr};
    QAction          *stayOnTopAction_ {nullptr};
    QSettings         settings_;
    ActionExecutor    actionExecutor_;
    DerflaActionList  dal_;
    DerflaActionList  dalDonate_;
    void              centerToScreen(QWidget *widget);
    void              createCommonActions();
    void              createDonateDerflaActions();
};

inline DerflaApp *derflaApp {nullptr};

#endif // DERFLAAPP_H
