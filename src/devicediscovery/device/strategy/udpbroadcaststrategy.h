#pragma once

#include <qobject.h>
#include <qudpsocket.h>
#include <qregularexpression.h>
#include <qnetworkinterface.h>

#include "../ibroadcaststrategy.h"
#include <devicediscovery/comm/global.h>

DEVICE_DISCOVERY_BEGIN_NAMESPACE

class DEVICE_DISCOVERY_EXPORT UdpBroadCastStrategy : public QObject, public IBroadcastStrategy {
public:
    explicit UdpBroadCastStrategy(QObject* parent = nullptr);

    void addInterfaceCheck(const QRegularExpression& regex);
    void bindPort(int listenPort);

    bool isAvailable() override;
    void reset() override;
    void broadcast() override;
    QString name() const override;

protected:
    QList<QRegularExpression> acceptableInterfaces;
    int udpListenPort = -1;

    QHash<int, QNetworkInterface> cachedInterface;
    QHash<int, QList<QUdpSocket*>> sockets;

    struct SocketData {
        QHostAddress from;
        quint16 port;
        QByteArray data;
    };

protected:
    QHash<int, QNetworkInterface> enumValidNetworkEntries() const;
    bool isTargetInterface(const QString& name) const;
    static QList<struct DeviceNetworkEntry> createNetworkEntry(const QNetworkInterface& networkInterface);

protected:
    virtual void bindSocket(QUdpSocket* socket, const QNetworkInterface& networkInterface);
    virtual void solveSocketData(QUdpSocket* socket, const QList<SocketData>& data, const QNetworkInterface& networkInterface);

private:
    static QList<SocketData> receiveDataFromSocket(QUdpSocket* socket);
};

class UdpBroadCastStrategyBuilder {
public:
    UdpBroadCastStrategyBuilder() {
        strategy = new UdpBroadCastStrategy;
    }

    UdpBroadCastStrategyBuilder& deviceId(const QString& id) {
        strategy->deviceId = id;
        return *this;
    }

    UdpBroadCastStrategyBuilder& deviceName(const QString& name) {
        strategy->deviceName = name;
        return *this;
    }

    UdpBroadCastStrategyBuilder& bindEth() {
        strategy->addInterfaceCheck(QRegularExpression("^eth"));
        return *this;
    }

    UdpBroadCastStrategyBuilder& bindWifi() {
        strategy->addInterfaceCheck(QRegularExpression("^wlan"));
        return *this;
    }

    UdpBroadCastStrategyBuilder& bindUsb() {
        strategy->addInterfaceCheck(QRegularExpression("^usb"));
        return *this;
    }

    UdpBroadCastStrategyBuilder& bindInterface(const QString& reg) {
        strategy->addInterfaceCheck(QRegularExpression(reg));
        return *this;
    }

    UdpBroadCastStrategyBuilder& listenOn(int port) {
        strategy->bindPort(port);
        return *this;
    }

    UdpBroadCastStrategy* build() const {
        return strategy;
    }

private:
    UdpBroadCastStrategy* strategy;
};

DEVICE_DISCOVERY_END_NAMESPACE
