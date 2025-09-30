#pragma once

#include <qobject.h>

#include "devicerecord.h"

namespace DeviceDiscovery {
    class IDiscoveryStrategy : public QObject {
        Q_OBJECT

    public:
        explicit IDiscoveryStrategy(QObject* parent = nullptr): QObject(parent) {}
        virtual ~IDiscoveryStrategy() = default;

        virtual void notify() = 0;

    signals:
        void deviceFound(const DeviceRecord& rec);
        void errorOccurred(const QString& err);
        void finished();

    protected:
        QString signKey;

        friend class UdpMulticastStrategyBuilder;
    };
}