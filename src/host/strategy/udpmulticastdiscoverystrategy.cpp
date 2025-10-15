#include "udpmulticastdiscoverystrategy.h"

#include "hmac.h"
#include "discoveryrequest.h"

namespace DeviceDiscovery {
    UdpMulticastDiscoveryStrategy::UdpMulticastDiscoveryStrategy(QObject* parent)
        : UdpBroadcastDiscoveryStrategy(parent) {
    }

    void UdpMulticastDiscoveryStrategy::setBroadcastAddress(const QHostAddress& address) {
        broadcastAddress = address;
    }

    void UdpMulticastDiscoveryStrategy::bindSocket(QUdpSocket* socket, const QNetworkInterface& networkInterface) {
        qInfo() << "Try bind multicast socket, networkInterface:" << networkInterface.name();
        if (socket->bind(QHostAddress::AnyIPv4, 0, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
            socket->setMulticastInterface(networkInterface);
            if (!socket->joinMulticastGroup(broadcastAddress, networkInterface)) {
                qWarning() << "Failed to join multicast group, address:" << broadcastAddress << "networkInterface:" << networkInterface.name();
            }
        } else {
            qWarning() << "Failed to bind multicast socket!";
        }
    }

    void UdpMulticastDiscoveryStrategy::sendRequest(const QByteArray& request) {
        for (const auto& it : sockets) {
            for (const auto& socket : it) {
                socket->writeDatagram(request, broadcastAddress, udpBroadcastPort);
            }
        }
    }
}
