#pragma once

#include <qobject.h>
#include <devicediscovery/comm/global.h>

#include "udpbroadcaststrategy.h"

DEVICE_DISCOVERY_BEGIN_NAMESPACE

class DEVICE_DISCOVERY_EXPORT UdpMulticastBroadCastStrategy : public UdpBroadCastStrategy {
public:
    explicit UdpMulticastBroadCastStrategy(QObject* parent = nullptr);

    void setBroadcastAddress(const QHostAddress& address);
    void broadcast() override;
    QString name() const override;

protected:
    void bindSocket(QUdpSocket* socket, const QNetworkInterface& networkInterface) override;

private:
    QHostAddress broadcastAddress;
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
        strategy->bindPort(port);
        return *this;
    }

    UdpMulticastBroadCastStrategy* build() const {
        return strategy;
    }

private:
    UdpMulticastBroadCastStrategy* strategy;
};

DEVICE_DISCOVERY_END_NAMESPACE