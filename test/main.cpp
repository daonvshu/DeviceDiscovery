#include <qcoreapplication.h>

#ifndef Q_OS_WIN
#define TEST_BROADCAST
#endif

#ifdef TEST_BROADCAST
#include <discoveryservice.h>
#include <strategy/udpmulticastbroadcaststrategy.h>
#else
#include <discoverymanager.h>
#include <strategy/udpmulticastdiscoverystrategy.h>
#endif

using namespace DeviceDiscovery;

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

#ifdef TEST_BROADCAST
    DiscoveryService::addStrategy(
        UdpMulticastStrategyBuilder()
            .bindAddress("239.0.1.1")
            .listenOn(19288)
            .bindInterface("^wlp")
            .build()
    );

    DiscoveryService::start();
#else
    DiscoveryManager::addStrategy(
        UdpMulticastStrategyBuilder()
            .bindAddress("239.0.1.1")
            .notifyOn(19288)
            .build()
    );

    DiscoveryManager::startScan(3000, [] (const QList<DeviceRecord>& records) {
        qInfo() << "get record size:" << records.size();
    });
#endif
    return app.exec();
}