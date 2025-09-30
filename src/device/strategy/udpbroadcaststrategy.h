#pragma once

#include <qobject.h>
#include <qudpsocket.h>
#include <qregularexpression.h>

#include "../ibroadcaststrategy.h"

#include <devicerecord.h>
#include <qnetworkinterface.h>

namespace DeviceDiscovery {
    class UdpBroadCastStrategy : public QObject, public IBroadcastStrategy {
    public:
        explicit UdpBroadCastStrategy(QObject* parent = nullptr);

        void addInterfaceCheck(const QRegularExpression& regex);
        void setBroadcastAddress(const QHostAddress& address);
        void bindPort(int listenPort, int broadcastPort);

        bool isAvailable() override;

    protected:
        QList<QRegularExpression> acceptableInterfaces;
        QHostAddress broadcastAddress;
        int listenPort = -1;
        int broadcastPort = -1;

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
        static QList<DeviceNetworkEntry> createNetworkEntry(const QNetworkInterface& networkInterface);

    protected:
        virtual void bindSocket(QUdpSocket* socket, const QNetworkInterface& networkInterface) = 0;
        virtual void solveSocketData(QUdpSocket* socket, const QList<SocketData>& data, const QNetworkInterface& networkInterface) = 0;

    private:
        static QList<SocketData> receiveDataFromSocket(QUdpSocket* socket);
    };
}
