#pragma once

#include <qobject.h>
#include <qudpsocket.h>
#include <qnetworkinterface.h>

#include "../idiscoverystrategy.h"

namespace DeviceDiscovery {
    class UdpDiscoveryStrategy : public IDiscoveryStrategy {
    public:
        explicit UdpDiscoveryStrategy(QObject* parent = nullptr);

        void setBroadcastAddress(const QHostAddress& address);
        void bindPort(int listenPort, int broadcastPort);

        void notify() override;

    protected:
        QHostAddress broadcastAddress;
        int broadcastPort = -1;

        QHash<int, QNetworkInterface> cachedInterface;
        QHash<int, QList<QUdpSocket*>> sockets;

        struct SocketData {
            QHostAddress from;
            quint16 port;
            QByteArray data;
        };

    protected:
        virtual void bindSocket(QUdpSocket* socket, const QNetworkInterface& networkInterface) = 0;
        virtual void solveSocketData(QUdpSocket* socket, const QList<SocketData>& data, const QNetworkInterface& networkInterface) = 0;

    private:
        static QList<SocketData> receiveDataFromSocket(QUdpSocket* socket);
    };
}