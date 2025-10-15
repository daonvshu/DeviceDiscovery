#pragma once

#include <qobject.h>

namespace DeviceDiscovery {
    class IBroadcastStrategy {
    public:
        virtual ~IBroadcastStrategy() = default;
        virtual bool isAvailable() = 0;
        virtual void broadcast() = 0;
        virtual QString name() const = 0;
        virtual void reset() = 0;

    protected:
        QString deviceId;
        QString deviceName;
        QString signKey;

        friend class DiscoveryService;
        friend class UdpBroadCastStrategyBuilder;
        friend class UdpMulticastStrategyBuilder;
    };
}