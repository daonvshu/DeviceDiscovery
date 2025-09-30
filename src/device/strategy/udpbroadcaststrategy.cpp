#include "udpbroadcaststrategy.h"

namespace DeviceDiscovery {
    UdpBroadCastStrategy::UdpBroadCastStrategy(QObject* parent)
        : QObject(parent) {
    }

    void UdpBroadCastStrategy::addInterfaceCheck(const QRegularExpression& regex) {
        acceptableInterfaces << regex;
    }

    void UdpBroadCastStrategy::setBroadcastAddress(const QHostAddress& address) {
        broadcastAddress = address;
    }

    void UdpBroadCastStrategy::bindPort(int listenPort, int broadcastPort) {
        this->listenPort = listenPort;
        this->broadcastPort = broadcastPort;
    }

    bool UdpBroadCastStrategy::isAvailable() {
        if (listenPort == -1 || broadcastPort == -1) {
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
        for (const auto& regex : acceptableInterfaces) {
            if (regex.match(name).hasMatch()) {
                return true;
            }
        }
        return false;
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

    QList<UdpBroadCastStrategy::SocketData> UdpBroadCastStrategy::receiveDataFromSocket(QUdpSocket* socket) {
        QList<SocketData> data;
        while (socket->hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(socket->pendingDatagramSize());
            QHostAddress sender;
            quint16 senderPort;
            socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
            data << SocketData{ sender, senderPort, datagram };
            qInfo() << "Received data from " << sender.toString() << ":" << senderPort;
        }
        return data;
    }
}
