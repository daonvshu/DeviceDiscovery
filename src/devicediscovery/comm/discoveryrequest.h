#pragma once

#include "datakey.h"
#include "global.h"

DEVICE_DISCOVERY_BEGIN_NAMESPACE

struct DiscoveryRequest : DataDumpInterface {
    DATA_KEY(QString, requestType); //network
    DATA_KEY(QString, nonce);
    DATA_KEY(qint64, ts);

    QList<DataReadInterface*> prop() override {
        return { &requestType, &nonce, &ts };
    }
};

DEVICE_DISCOVERY_END_NAMESPACE