#include "discoverymanager.h"

#include <qtimer.h>

DEVICE_DISCOVERY_BEGIN_NAMESPACE

struct DiscoveryManagerData {
    QList<IDiscoveryStrategy*> strategies;
    std::function<void(const QList<DeviceRecord>&)> callback;
    QHash<QString, DeviceRecord> records;
    QTimer timeoutTimer;
};


void DiscoveryManager::addStrategy(IDiscoveryStrategy* strategy) {
    instance().data->strategies << strategy;
}

void DiscoveryManager::setStrategiesKey(const QString& key) {
    for (auto strategy : instance().data->strategies) {
        strategy->signKey = key;
    }
}

void DiscoveryManager::startScan(int scanTimeMs, const std::function<void(const QList<DeviceRecord>&)>& callback) {
    instance().data->records.clear();
    instance().data->callback = callback;
    instance().data->timeoutTimer.start(scanTimeMs);
    instance().sendNotify();
}

void DiscoveryManager::cancel() {
    instance().data->timeoutTimer.stop();
}

DiscoveryManager::DiscoveryManager() {
    data.reset(new DiscoveryManagerData);
    data->timeoutTimer.setSingleShot(true);
    connect(&data->timeoutTimer, &QTimer::timeout, this, [this] {
        for (auto strategy : data->strategies) {
            disconnect(strategy, &IDiscoveryStrategy::deviceFound, this, &DiscoveryManager::deviceFound);
        }
        auto records = data->records.values();
        QSet<QString> existIps;
        for (int i = records.size() - 1; i >= 0; --i) {
            auto targetIp = records[i].feedbackEntry().ip();
            if (existIps.contains(targetIp)) {
                records.removeAt(i);
            } else {
                existIps.insert(targetIp);
            }
        }
        data->callback(records);
    });
}

DiscoveryManager& DiscoveryManager::instance() {
    static DiscoveryManager instance;
    return instance;
}

void DiscoveryManager::sendNotify() const {
    for (auto strategy : data->strategies) {
        connect(strategy, &IDiscoveryStrategy::deviceFound, this, &DiscoveryManager::deviceFound);
        strategy->notify();
    }
}

void DiscoveryManager::deviceFound(const DeviceRecord& rec) const {
    data->records[rec.sig()] = rec;
}

DEVICE_DISCOVERY_END_NAMESPACE
