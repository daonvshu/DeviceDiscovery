#include "udpmulticastbroadcaststrategy.h"

#include <qnetworkdatagram.h>
#include <qdebug.h>

#include "discoveryrequest.h"
#include "hmac.h"

namespace DeviceDiscovery {
    UdpMulticastBroadCastStrategy::UdpMulticastBroadCastStrategy(QObject* parent)
        : UdpBroadCastStrategy(parent)
    {
    }

    void UdpMulticastBroadCastStrategy::broadcast() {

    }

    QString UdpMulticastBroadCastStrategy::name() const {
        return "udp multicast";
    }

    void UdpMulticastBroadCastStrategy::bindSocket(QUdpSocket* socket, const QNetworkInterface& networkInterface) {
        qInfo() << "Try bind socket" << "port:" << udpListenPort << "networkInterface:" << networkInterface.name();
        if (socket->bind(QHostAddress::AnyIPv4, udpListenPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
            socket->setMulticastInterface(networkInterface);
            socket->leaveMulticastGroup(broadcastAddress, networkInterface);
            if (!socket->joinMulticastGroup(broadcastAddress, networkInterface)) {
                qWarning() << "Failed to join multicast group, address:" << broadcastAddress << "networkInterface:" << networkInterface.name();
            }
        } else {
            qWarning() << "Failed to bind socket!";
        }
    }

    void UdpMulticastBroadCastStrategy::solveSocketData(QUdpSocket* socket, const QList<SocketData>& data, const QNetworkInterface& networkInterface) {
        if (data.isEmpty()) {
            return;
        }

        auto document = QJsonDocument::fromJson(data.first().data);
        if (document.isNull() || !document.isObject()) {
            return;
        }
        DiscoveryRequest request;
        request.fromJson(document.object());

        if (request.requestType() != "network") {
            return;
        }

        DeviceRecord record;
        record.deviceId = deviceId;
        record.deviceName = deviceName;
        auto curEntries = createNetworkEntry(networkInterface);
        record.feedbackEntry = curEntries.isEmpty() ? DeviceNetworkEntry() : curEntries.first();
        for (const auto& it : cachedInterface) {
            record.entries().append(createNetworkEntry(it));
        }
        record.nonce = request.nonce();
        record.ts = QDateTime::currentMSecsSinceEpoch();
        if (!signKey.isEmpty()) {
            record.sig = hmac(signKey.toLatin1(), record.getSigStr().toLatin1(), QCryptographicHash::Sha3_256).toBase64();
        }
        auto feedback = QJsonDocument(record.dumpToJson()).toJson();
        QNetworkDatagram datagram;
        datagram.setDestination(data.first().from, data.first().port);
        datagram.setData(feedback);
        datagram.setInterfaceIndex(networkInterface.index());
        socket->writeDatagram(datagram);
    }
}
