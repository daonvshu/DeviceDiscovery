#pragma once

#include <qobject.h>
#include <qtimer.h>
#include <devicediscovery/comm/global.h>

#include "ibroadcaststrategy.h"

DEVICE_DISCOVERY_BEGIN_NAMESPACE

class DEVICE_DISCOVERY_EXPORT DiscoveryService : public QObject {
public:
    static void addStrategy(IBroadcastStrategy* strategy);
    static void setStrategiesKey(const QString& key);
    static void start();
    static void stop();
    static void reset();

private:
    explicit DiscoveryService();
    static DiscoveryService& instance();

    void checkDiscovery();

private:
    QList<IBroadcastStrategy*> strategies;
    QTimer* timer;
};

DEVICE_DISCOVERY_END_NAMESPACE