#include "discoveryservice.h"

DEVICE_DISCOVERY_BEGIN_NAMESPACE

void DiscoveryService::addStrategy(IBroadcastStrategy* strategy) {
    instance().strategies.append(strategy);
}

void DiscoveryService::setStrategiesKey(const QString& key) {
    for (auto& strategy : instance().strategies) {
        strategy->signKey = key;
    }
}

void DiscoveryService::start() {
    instance().timer->start();
}

void DiscoveryService::stop() {
    instance().timer->stop();
}

void DiscoveryService::reset() {
    for (const auto& strategy : instance().strategies) {
        strategy->reset();
    }
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

DEVICE_DISCOVERY_END_NAMESPACE
