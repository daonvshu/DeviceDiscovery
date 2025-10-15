#pragma once

#include <qobject.h>

#include "udpbroadcastdiscoverystrategy.h"

namespace DeviceDiscovery {
    class UdpMulticastDiscoveryStrategy : public UdpBroadcastDiscoveryStrategy {
    public:
        explicit UdpMulticastDiscoveryStrategy(QObject* parent = nullptr);

        void setBroadcastAddress(const QHostAddress& address);

    protected:
        void bindSocket(QUdpSocket* socket, const QNetworkInterface& networkInterface) override;
        void sendRequest(const QByteArray& request) override;

    private:
        QHostAddress broadcastAddress;
    };

    class UdpMulticastStrategyBuilder {
    public:
        UdpMulticastStrategyBuilder() {
            strategy = new UdpMulticastDiscoveryStrategy;
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