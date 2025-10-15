#include "udpdiscoverystrategy.h"

#include "../interfacetools.h"

#include <discoveryrequest.h>
#include <hmac.h>

namespace DeviceDiscovery {
    UdpDiscoveryStrategy::UdpDiscoveryStrategy(QObject* parent)
        : IDiscoveryStrategy(parent)
    {
    }

    void UdpDiscoveryStrategy::setBroadcastAddress(const QHostAddress& address) {
        broadcastAddress = address;
    }

    void UdpDiscoveryStrategy::bindPort(int broadcastPort) {
        udpBroadcastPort = broadcastPort;
    }

    void UdpDiscoveryStrategy::notify() {
        if (udpBroadcastPort == -1) {
            return;
        }
        auto physicalInterfaces = InterfaceTools::getPhysicalInterfaces();
        QHash<int, QNetworkInterface> currentInterface;
        for (const auto& it : physicalInterfaces) {
            currentInterface.insert(it.index(), it);
        }
        //check added
        QList<QNetworkInterface> added;
        for (const auto& it : currentInterface) {
            if (!cachedInterface.contains(it.index())) {
                added << it;
            }
        }
        //check removed
        QList<int> removed;
        for (const auto& it : cachedInterface) {
            if (!currentInterface.contains(it.index())) {
                removed << it.index();
            }
        }
        //remove old socket from removed
        for (const auto& it : removed) {
            qDeleteAll(sockets.take(it));
        }

        //create new socket for added
        for (const auto& it : added) {
            QList<QUdpSocket*> localSockets;
            if (!it.flags().testFlag(QNetworkInterface::IsUp) ||
                !it.flags().testFlag(QNetworkInterface::CanMulticast) ||
                it.flags().testFlag(QNetworkInterface::IsLoopBack)
            ) continue;

            auto socket = new QUdpSocket(this);
            connect(socket, &QUdpSocket::readyRead, this, [this, socket, it] {
                auto data = receiveDataFromSocket(socket);
                if (data.isEmpty()) {
                    return;
                }
                solveSocketData(socket, data, it);
            });
            bindSocket(socket, it);
            localSockets << socket;
            sockets[it.index()] = localSockets;
        }

        DiscoveryRequest request;
        request.requestType = "network";
        request.nonce = generateNonce().toHex();
        request.ts = QDateTime::currentMSecsSinceEpoch();
        for (const auto& it : sockets) {
            for (const auto& socket : it) {
                auto data = QJsonDocument(request.dumpToJson()).toJson(QJsonDocument::Compact);
                socket->writeDatagram(data, broadcastAddress, udpBroadcastPort);
            }
        }
    }

    QList<UdpDiscoveryStrategy::SocketData> UdpDiscoveryStrategy::receiveDataFromSocket(QUdpSocket* socket) {
        QList<SocketData> data;
        while (socket->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(static_cast<int>(socket->pendingDatagramSize()));
            QHostAddress sender;
            quint16 senderPort;
            socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
            data << SocketData{ sender, senderPort, datagram };
            qInfo() << "Received data from " << sender.toString() << ":" << senderPort;
        }
        return data;
    }
}
