#pragma once

#include <qobject.h>
#include <qnetworkinterface.h>

namespace DeviceDiscovery {
    class InterfaceTools {
    public:
        static QList<QNetworkInterface> getPhysicalInterfaces();
        static bool testInterfaceIsPhysicalAdapter(const QNetworkInterface& i);
    };
}