#pragma once

#include <qobject.h>
#include <qsharedpointer.h>

#include <devicediscovery/comm/devicerecord.h>
#include <devicediscovery/comm/global.h>

#include "idiscoverystrategy.h"

DEVICE_DISCOVERY_BEGIN_NAMESPACE

struct DiscoveryManagerData;
class DEVICE_DISCOVERY_EXPORT DiscoveryManager : public QObject {
public:
    static void addStrategy(IDiscoveryStrategy* strategy);
    static void setStrategiesKey(const QString& key);
    static void startScan(int scanTimeMs, const std::function<void(const QList<DeviceRecord>&)>& callback);
    static void cancel();

private:
    explicit DiscoveryManager();
    static DiscoveryManager& instance();

    void sendNotify() const;

private:
    void deviceFound(const DeviceRecord& rec) const;

private:
    QSharedPointer<DiscoveryManagerData> data;
};

DEVICE_DISCOVERY_END_NAMESPACE