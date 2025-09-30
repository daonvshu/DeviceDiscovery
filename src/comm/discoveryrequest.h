#pragma once

#include "datakey.h"

namespace DeviceDiscovery {

    struct DiscoveryRequest : DataDumpInterface {
        DATA_KEY(QString, requestType); //network
        DATA_KEY(QString, nonce);
        DATA_KEY(qint64, ts);

        QList<DataReadInterface*> prop() override {
            return { &requestType, &nonce, &ts };
        }
    };
}