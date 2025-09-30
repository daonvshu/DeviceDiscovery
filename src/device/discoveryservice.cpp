#include "discoveryservice.h"

namespace DeviceDiscovery {
    void DiscoveryService::addStrategy(IBroadcastStrategy* strategy) {
        instance().strategies.append(strategy);
    }

    void DiscoveryService::start() {
        instance().timer->start();
    }

    void DiscoveryService::stop() {
        instance().timer->stop();
    }

    DiscoveryService::DiscoveryService() {
        timer = new QTimer(this);
        timer->setInterval(3000);
        connect(timer, &QTimer::timeout, this, &DiscoveryService::checkDiscovery);
    }

    DiscoveryService& DiscoveryService::instance() {
        static DiscoveryService instance;
        return instance;
    }

    void DiscoveryService::checkDiscovery() {
        for (const auto& strategy : strategies) {
            if (strategy->isAvailable()) {
                strategy->broadcast();
            }
        }
    }
}
