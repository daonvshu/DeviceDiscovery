#pragma once

#include <qobject.h>
#include <qudpsocket.h>
#include <qnetworkinterface.h>

#include <devicediscovery/comm/global.h>

#include "../idiscoverystrategy.h"

DEVICE_DISCOVERY_BEGIN_NAMESPACE

class DEVICE_DISCOVERY_EXPORT UdpBroadcastDiscoveryStrategy : public IDiscoveryStrategy {
public:
    explicit UdpBroadcastDiscoveryStrategy(QObject* parent = nullptr);

    void bindPort(int broadcastPort);

    void notify() override;

protected:
    int udpBroadcastPort = -1;

    QHash<int, QNetworkInterface> cachedInterface;
    QHash<int, QList<QUdpSocket*>> sockets;

    struct SocketData {
        QHostAddress from;
        quint16 port;
        QByteArray data;
    };

protected:
    virtual void bindSocket(QUdpSocket* socket, const QNetworkInterface& networkInterface);
    virtual void solveSocketData(QUdpSocket* socket, const QList<SocketData>& data, const QNetworkInterface& networkInterface);
    virtual void sendRequest(const QByteArray& request);

private:
    static QList<SocketData> receiveDataFromSocket(QUdpSocket* socket);
};

class UdpBroadcastStrategyBuilder {
public:
    UdpBroadcastStrategyBuilder() {
        strategy = new UdpBroadcastDiscoveryStrategy;
    }

    UdpBroadcastStrategyBuilder& notifyOn(int port) {
        strategy->bindPort(port);
        return *this;
    }

    UdpBroadcastDiscoveryStrategy* build() const {
        return strategy;
    }

private:
    UdpBroadcastDiscoveryStrategy* strategy;
};

DEVICE_DISCOVERY_END_NAMESPACE