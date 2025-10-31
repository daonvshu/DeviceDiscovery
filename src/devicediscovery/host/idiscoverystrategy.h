#pragma once

#include <qobject.h>

#include <devicediscovery/comm/devicerecord.h>
#include <devicediscovery/comm/global.h>

DEVICE_DISCOVERY_BEGIN_NAMESPACE

class DEVICE_DISCOVERY_EXPORT IDiscoveryStrategy : public QObject {
    Q_OBJECT

public:
    explicit IDiscoveryStrategy(QObject* parent = nullptr): QObject(parent) {}
    virtual ~IDiscoveryStrategy() = default;

    virtual void notify() = 0;

signals:
    void deviceFound(const DeviceRecord& rec);
    void errorOccurred(const QString& err);
    void finished();

protected:
    QString signKey;

    friend class DiscoveryManager;
};

DEVICE_DISCOVERY_END_NAMESPACE