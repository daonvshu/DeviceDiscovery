#include "udpbroadcastdiscoverystrategy.h"

#include "../interfacetools.h"

#include <devicediscovery/comm/discoveryrequest.h>
#include <devicediscovery/comm/hmac.h>
#include <qnetworkproxy.h>
#include <qdatetime.h>

DEVICE_DISCOVERY_BEGIN_NAMESPACE

UdpBroadcastDiscoveryStrategy::UdpBroadcastDiscoveryStrategy(QObject* parent)
    : IDiscoveryStrategy(parent)
{
}

void UdpBroadcastDiscoveryStrategy::bindPort(int broadcastPort) {
    udpBroadcastPort = broadcastPort;
}

void UdpBroadcastDiscoveryStrategy::notify() {
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
        socket->setProxy(QNetworkProxy::NoProxy);
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
    auto data = QJsonDocument(request.dumpToJson()).toJson(QJsonDocument::Compact);
    sendRequest(data);
}

void UdpBroadcastDiscoveryStrategy::bindSocket(QUdpSocket* socket, const QNetworkInterface& networkInterface) {
    qInfo() << "Try bind broadcast socket, networkInterface:" << networkInterface.name();
    if (!socket->bind(QHostAddress::AnyIPv4, 0, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        qWarning() << "Failed to bind broadcast socket!";
    }

    auto entries = networkInterface.addressEntries();
    for (const auto& entry : entries) {
        if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
            socket->setProperty("broadcastAddr", entry.broadcast().toIPv4Address());
        }
    }
}

void UdpBroadcastDiscoveryStrategy::solveSocketData(QUdpSocket* socket, const QList<SocketData>& data, const QNetworkInterface& networkInterface) {
    for (const auto& socketData : data) {
        qInfo() << "Receive from:" << socketData.from << "port:" << socketData.port;
        auto document = QJsonDocument::fromJson(socketData.data);
        if (document.isNull() || !document.isObject()) {
            continue;
        }
        DeviceRecord record;
        record.fromJson(document.object());

        if (!signKey.isEmpty()) {
            auto curSig = hmac(signKey.toLatin1(), record.getSigStr().toLatin1(), QCryptographicHash::Sha3_256);
            if (curSig.toBase64() != record.sig()) {
                qInfo() << "Invalid signature, record:" << record.deviceName();
                continue;
            }
        }
        emit deviceFound(record);
    }
}

void UdpBroadcastDiscoveryStrategy::sendRequest(const QByteArray& request) {
    for (const auto& it : sockets) {
        for (const auto& socket : it) {
            auto broadcastAddress = socket->property("broadcastAddr").toUInt();
            socket->writeDatagram(request, QHostAddress(broadcastAddress), udpBroadcastPort);
        }
    }
}

QList<UdpBroadcastDiscoveryStrategy::SocketData> UdpBroadcastDiscoveryStrategy::receiveDataFromSocket(QUdpSocket* socket) {
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

DEVICE_DISCOVERY_END_NAMESPACE
