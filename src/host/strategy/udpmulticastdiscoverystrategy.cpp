#include "udpmulticastdiscoverystrategy.h"

namespace DeviceDiscovery {
    UdpMulticastDiscoveryStrategy::UdpMulticastDiscoveryStrategy(QObject* parent)
        : UdpDiscoveryStrategy(parent) {
    }

    void UdpMulticastDiscoveryStrategy::bindSocket(QUdpSocket* socket, const QNetworkInterface& networkInterface) {
        qInfo() << "Try bind socket, networkInterface:" << networkInterface.name();
        if (socket->bind(QHostAddress::AnyIPv4, 0, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
            socket->setMulticastInterface(networkInterface);
            if (!socket->joinMulticastGroup(broadcastAddress, networkInterface)) {
                qWarning() << "Failed to join multicast group, address:" << broadcastAddress << "networkInterface:" << networkInterface.name();
            }
        } else {
            qWarning() << "Failed to bind socket!";
        }
    }

    void UdpMulticastDiscoveryStrategy::solveSocketData(QUdpSocket* socket, const QList<SocketData>& data, const QNetworkInterface& networkInterface) {
        for (const auto& socketData : data) {
            qInfo() << "Receive from:" << socketData.from << "port:" << socketData.port;
            auto document = QJsonDocument::fromJson(socketData.data);
            if (document.isNull() || !document.isObject()) {
                continue;
            }
            DeviceRecord record;
            record.fromJson(document.object());
            emit deviceFound(record);
        }
    }
}
