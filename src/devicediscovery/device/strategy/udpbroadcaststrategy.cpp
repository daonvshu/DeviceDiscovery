#include "udpbroadcaststrategy.h"

#include <devicediscovery/comm/discoveryrequest.h>
#include <devicediscovery/comm/hmac.h>
#include <devicediscovery/comm/devicerecord.h>

#include <qnetworkdatagram.h>
#include <qdatetime.h>

DEVICE_DISCOVERY_BEGIN_NAMESPACE

UdpBroadCastStrategy::UdpBroadCastStrategy(QObject* parent)
    : QObject(parent) {
}

void UdpBroadCastStrategy::addInterfaceCheck(const QRegularExpression& regex) {
    acceptableInterfaces << regex;
}

void UdpBroadCastStrategy::bindPort(int listenPort) {
    udpListenPort = listenPort;
}

bool UdpBroadCastStrategy::isAvailable() {
    if (udpListenPort == -1) {
        return false;
    }

    auto currentInterface = enumValidNetworkEntries();
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

    cachedInterface = currentInterface;
    return !cachedInterface.isEmpty();
}

void UdpBroadCastStrategy::reset() {
    for (auto& it : sockets) {
        qDeleteAll(it);
    }
    sockets.clear();
    cachedInterface.clear();
}

void UdpBroadCastStrategy::broadcast() {
}

QString UdpBroadCastStrategy::name() const {
    return "udp broadcast";
}

QHash<int, QNetworkInterface> UdpBroadCastStrategy::enumValidNetworkEntries() const {
    QHash<int, QNetworkInterface> entries;
    auto interfaces = QNetworkInterface::allInterfaces();
    for (const auto& it : interfaces) {
        if (it.flags().testFlag(QNetworkInterface::IsUp) && it.flags().testFlag(QNetworkInterface::IsRunning)) {
            auto name = it.name();
            if (isTargetInterface(name)) {
                entries[it.index()] = it;
            }
        }
    }
    return entries;
}

bool UdpBroadCastStrategy::isTargetInterface(const QString& name) const {
    if (acceptableInterfaces.isEmpty()) {
        return true;
    }
    return std::any_of(acceptableInterfaces.begin(), acceptableInterfaces.end(), [&] (const QRegularExpression& regex) {
        return regex.match(name).hasMatch();
    });
}

QList<DeviceNetworkEntry> UdpBroadCastStrategy::createNetworkEntry(const QNetworkInterface& networkInterface) {
    QList<DeviceNetworkEntry> entries;
    for (const auto& address : networkInterface.addressEntries()) {
        if (address.ip().protocol() == QAbstractSocket::IPv4Protocol && !address.ip().isLoopback()) {
            DeviceNetworkEntry entry;
            entry.name = networkInterface.name();
            entry.ip = address.ip().toString();
            entry.netmask = address.netmask().toString();
            entry.mac = networkInterface.hardwareAddress();
            entry.broadcast = address.broadcast().toString();
            entries.append(entry);
        }
    }
    return entries;
}

void UdpBroadCastStrategy::bindSocket(QUdpSocket* socket, const QNetworkInterface& networkInterface) {
    qInfo() << "Try bind broadcast socket" << "port:" << udpListenPort << "networkInterface:" << networkInterface.name();
    if (!socket->bind(QHostAddress::AnyIPv4, udpListenPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        qWarning() << "Failed to bind broadcast socket!";
    }
}

void UdpBroadCastStrategy::solveSocketData(QUdpSocket* socket, const QList<SocketData>& data, const QNetworkInterface& networkInterface) {
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

QList<UdpBroadCastStrategy::SocketData> UdpBroadCastStrategy::receiveDataFromSocket(QUdpSocket* socket) {
    QList<SocketData> data;
    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(static_cast<int>(socket->pendingDatagramSize()));
        QHostAddress sender;
        quint16 senderPort;
        socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        data << SocketData{ sender, senderPort, datagram };
        //qInfo() << "Received data from " << sender.toString() << ":" << senderPort;
    }
    return data;
}

DEVICE_DISCOVERY_END_NAMESPACE
