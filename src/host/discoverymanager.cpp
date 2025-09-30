#include "discoverymanager.h"

#include <qtimer.h>

namespace DeviceDiscovery {
    struct DiscoveryManagerData {
        QList<IDiscoveryStrategy*> strategies;
        std::function<void(const QList<DeviceRecord>&)> callback;
        QHash<QString, DeviceRecord> records;
        QTimer timoutTimer;
    };


    void DiscoveryManager::addStrategy(IDiscoveryStrategy* strategy) {
        instance().data->strategies << strategy;
    }

    void DiscoveryManager::startScan(int scanTimeMs, const std::function<void(const QList<DeviceRecord>&)>& callback) {
        instance().data->callback = callback;
        instance().data->timoutTimer.start(scanTimeMs);
        instance().sendNotify();
    }

    void DiscoveryManager::cancel() {
        instance().data->timoutTimer.stop();
    }

    DiscoveryManager::DiscoveryManager() {
        data.reset(new DiscoveryManagerData);
        data->timoutTimer.setSingleShot(true);
        connect(&data->timoutTimer, &QTimer::timeout, this, [this] {
            for (auto strategy : data->strategies) {
                disconnect(strategy, &IDiscoveryStrategy::deviceFound, this, &DiscoveryManager::deviceFound);
            }
            data->callback(data->records.values());
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
}
