#pragma once

#include <qobject.h>
#include <qtimer.h>

#include "ibroadcaststrategy.h"

namespace DeviceDiscovery {
    class DiscoveryService : public QObject {
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
}