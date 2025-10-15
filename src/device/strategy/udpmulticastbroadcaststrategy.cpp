#include "udpmulticastbroadcaststrategy.h"

#include <qdebug.h>

namespace DeviceDiscovery {
    UdpMulticastBroadCastStrategy::UdpMulticastBroadCastStrategy(QObject* parent)
        : UdpBroadCastStrategy(parent)
    {
    }

    void UdpMulticastBroadCastStrategy::setBroadcastAddress(const QHostAddress& address) {
        broadcastAddress = address;
    }

    void UdpMulticastBroadCastStrategy::broadcast() {
    }

    QString UdpMulticastBroadCastStrategy::name() const {
        return "udp multicast";
    }

    void UdpMulticastBroadCastStrategy::bindSocket(QUdpSocket* socket, const QNetworkInterface& networkInterface) {
        qInfo() << "Try bind multicast socket" << "port:" << udpListenPort << "networkInterface:" << networkInterface.name();
        if (socket->bind(QHostAddress::AnyIPv4, udpListenPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
            socket->setMulticastInterface(networkInterface);
            socket->leaveMulticastGroup(broadcastAddress, networkInterface);
            if (!socket->joinMulticastGroup(broadcastAddress, networkInterface)) {
                qWarning() << "Failed to join multicast group, address:" << broadcastAddress << "networkInterface:" << networkInterface.name();
            }
        } else {
            qWarning() << "Failed to bind multicast socket!";
        }
    }
}
