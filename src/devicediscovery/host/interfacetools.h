#pragma once

#include <qobject.h>
#include <qnetworkinterface.h>

#include <devicediscovery/comm/global.h>

DEVICE_DISCOVERY_BEGIN_NAMESPACE

class DEVICE_DISCOVERY_EXPORT InterfaceTools {
public:
    static QList<QNetworkInterface> getPhysicalInterfaces();
    static bool testInterfaceIsPhysicalAdapter(const QNetworkInterface& i);
};

DEVICE_DISCOVERY_END_NAMESPACE