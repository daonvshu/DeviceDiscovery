#pragma once

#include "datakey.h"
#include "global.h"

DEVICE_DISCOVERY_BEGIN_NAMESPACE

struct DeviceNetworkEntry : DataDumpInterface {
    DATA_KEY(QString, name);
    DATA_KEY(QString, ip);
    DATA_KEY(QString, netmask);
    DATA_KEY(QString, mac);
    DATA_KEY(QString, broadcast);

    QList<DataReadInterface*> prop() override {
        return { &name, &ip, &netmask, &mac, &broadcast };
    }

    DeviceNetworkEntry& operator=(const DeviceNetworkEntry& other) {
        name = other.name();
        ip = other.ip();
        netmask = other.netmask();
        mac = other.mac();
        broadcast = other.broadcast();
        return *this;
    }

    QString getSigStr() const {
        return name() + ip() + netmask() + mac() + broadcast();
    }
};

struct DeviceRecord : DataDumpInterface {
    DATA_KEY(QString, deviceId);
    DATA_KEY(QString, deviceName);
    DATA_KEY(DeviceNetworkEntry, feedbackEntry);
    DATA_KEY(QList<DeviceNetworkEntry>, entries);
    DATA_KEY(QString, nonce);
    DATA_KEY(qint64, ts);
    DATA_KEY(QString, sig);

    QList<DataReadInterface*> prop() override {
        return { &deviceId, &deviceName, &feedbackEntry, &entries, &nonce, &ts, &sig };
    }

    DeviceRecord& operator=(const DeviceRecord& other) {
        deviceId = other.deviceId();
        deviceName = other.deviceName();
        feedbackEntry = other.feedbackEntry();
        entries = other.entries();
        nonce = other.nonce();
        ts = other.ts();
        sig = other.sig();
        return *this;
    }

    QString getSigStr() const {
        QString str;
        str += deviceId() + deviceName() + feedbackEntry().getSigStr();
        for (auto entry : entries()) {
            str += entry.getSigStr();
        }
        str += nonce() + QString::number(ts());
        return str;
    }
};

DEVICE_DISCOVERY_END_NAMESPACE

Q_DECLARE_METATYPE(DeviceDiscovery::DeviceRecord)