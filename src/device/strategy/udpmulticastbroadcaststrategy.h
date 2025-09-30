#pragma once

#include <qobject.h>

#include "udpbroadcaststrategy.h"

namespace DeviceDiscovery {
    class UdpMulticastBroadCastStrategy : public UdpBroadCastStrategy {
    public:
        explicit UdpMulticastBroadCastStrategy(QObject* parent = nullptr);

        void broadcast() override;
        QString name() const override;

    protected:
        void bindSocket(QUdpSocket* socket, const QNetworkInterface& networkInterface) override;
        void solveSocketData(QUdpSocket* socket, const QList<SocketData>& data, const QNetworkInterface& networkInterface) override;
    };

    class UdpMulticastStrategyBuilder {
    public:
        UdpMulticastStrategyBuilder() {
            strategy = new UdpMulticastBroadCastStrategy;
        }

        UdpMulticastStrategyBuilder& deviceId(const QString& id) {
            strategy->deviceId = id;
            return *this;
        }

        UdpMulticastStrategyBuilder& deviceName(const QString& name) {
            strategy->deviceName = name;
            return *this;
        }

        UdpMulticastStrategyBuilder& key(const QString& key) {
            strategy->signKey = key;
            return *this;
        }

        UdpMulticastStrategyBuilder& bindEth() {
            strategy->addInterfaceCheck(QRegularExpression("^eth"));
            return *this;
        }

        UdpMulticastStrategyBuilder& bindWifi() {
            strategy->addInterfaceCheck(QRegularExpression("^wlan"));
            return *this;
        }

        UdpMulticastStrategyBuilder& bindUsb() {
            strategy->addInterfaceCheck(QRegularExpression("^usb"));
            return *this;
        }

        UdpMulticastStrategyBuilder& bindInterface(const QString& reg) {
            strategy->addInterfaceCheck(QRegularExpression(reg));
            return *this;
        }

        UdpMulticastStrategyBuilder& bindAddress(const QString& host) {
            strategy->setBroadcastAddress(QHostAddress(host));
            return *this;
        }

        UdpMulticastStrategyBuilder& listenOn(int port) {
            strategy->bindPort(port, 0);
            return *this;
        }

        UdpMulticastBroadCastStrategy* build() const {
            return strategy;
        }

    private:
        UdpMulticastBroadCastStrategy* strategy;
    };
}