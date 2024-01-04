#pragma once

#include <QJsonArray>
#include <QObject>

class Responser : public QObject
{
    Q_OBJECT

public:
    explicit Responser(QObject *parent = nullptr);
    void               increaseService();
    [[nodiscard]] bool hasService() const;

public slots:
    void receivedExplain(QJsonArray arr);
    void emptyExplain();

private:
    int        m_serviceCount {0};
    int        m_serviceReplied {0};
    QJsonArray m_replied;
    void       checkResult();
};