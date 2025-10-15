#include <qcoreapplication.h>

#ifndef Q_OS_WIN
#define TEST_BROADCAST
#endif

#ifdef TEST_BROADCAST
#include <discoveryservice.h>
#include <strategy/udpbroadcaststrategy.h>
#include <strategy/udpmulticastbroadcaststrategy.h>
#else
#include <discoverymanager.h>
#include <strategy/udpbroadcastdiscoverystrategy.h>
#include <strategy/udpmulticastdiscoverystrategy.h>
#endif

using namespace DeviceDiscovery;

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

#ifdef TEST_BROADCAST
    DiscoveryService::addStrategy(
        UdpBroadCastStrategyBuilder()
            .listenOn(19288)
            .bindEth()
            .bindInterface("^wlp")
            .build()
    );

    DiscoveryService::addStrategy(
        UdpMulticastStrategyBuilder()
            .bindAddress("239.0.1.1")
            .listenOn(19288)
            .bindEth()
            .bindInterface("^wlp")
            .build()
    );

    DiscoveryService::setStrategiesKey("xxxx");

    DiscoveryService::start();
#else
    DiscoveryManager::addStrategy(
        UdpBroadcastStrategyBuilder()
            .notifyOn(19288)
            .build()
    );

    DiscoveryManager::addStrategy(
        UdpMulticastStrategyBuilder()
            .bindAddress("239.0.1.1")
            .notifyOn(19288)
            .build()
    );

    DiscoveryManager::setStrategiesKey("xxxx");

    DiscoveryManager::startScan(3000, [] (const QList<DeviceRecord>& records) {
        qInfo() << "get record size:" << records.size();
    });
#endif
    return app.exec();
}