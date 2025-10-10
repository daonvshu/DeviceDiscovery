#pragma once

#include <qobject.h>

#include "udpdiscoverystrategy.h"

namespace DeviceDiscovery {
    class UdpMulticastDiscoveryStrategy : public UdpDiscoveryStrategy {
    public:
        explicit UdpMulticastDiscoveryStrategy(QObject* parent = nullptr);

    protected:
        void bindSocket(QUdpSocket* socket, const QNetworkInterface& networkInterface) override;
        void solveSocketData(QUdpSocket* socket, const QList<SocketData>& data, const QNetworkInterface& networkInterface) override;
    };

    class UdpMulticastStrategyBuilder {
    public:
        UdpMulticastStrategyBuilder() {
            strategy = new UdpMulticastDiscoveryStrategy;
        }

        UdpMulticastStrategyBuilder& key(const QString& key) {
            strategy->signKey = key;
            return *this;
        }

        UdpMulticastStrategyBuilder& bindAddress(const QString& host) {
            strategy->setBroadcastAddress(QHostAddress(host));
            return *this;
        }

        UdpMulticastStrategyBuilder& notifyOn(int port) {
            strategy->bindPort(port);
            return *this;
        }

        UdpMulticastDiscoveryStrategy* build() const {
            return strategy;
        }

    private:
        UdpMulticastDiscoveryStrategy* strategy;
    };
}