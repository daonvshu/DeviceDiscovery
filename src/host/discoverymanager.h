#pragma once

#include <qobject.h>
#include <qsharedpointer.h>

#include <devicerecord.h>

#include "idiscoverystrategy.h"

namespace DeviceDiscovery {
    struct DiscoveryManagerData;
    class DiscoveryManager : public QObject {
    public:
        static void addStrategy(IDiscoveryStrategy* strategy);
        static void setStrategiesKey(const QString& key);
        static void startScan(int scanTimeMs, const std::function<void(const QList<DeviceRecord>&)>& callback);
        static void cancel();

    private:
        explicit DiscoveryManager();
        static DiscoveryManager& instance();

        void sendNotify() const;

    private:
        void deviceFound(const DeviceRecord& rec) const;

    private:
        QSharedPointer<DiscoveryManagerData> data;
    };
}