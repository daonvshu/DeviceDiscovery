#include "interfacetools.h"

#include <qprocess.h>
#include <qsettings.h>

namespace DeviceDiscovery {
    QList<QNetworkInterface> InterfaceTools::getPhysicalInterfaces() {
        QList<QNetworkInterface> physicalInterfaces;
        auto interfaceList = QNetworkInterface::allInterfaces();
        for (const auto& i : interfaceList) {
            if (testInterfaceIsPhysicalAdapter(i)) {
                if (i.flags().testFlag(QNetworkInterface::IsUp) && i.flags().testFlag(QNetworkInterface::IsRunning)) {
                    physicalInterfaces << i;
                }
            }
        }
        return physicalInterfaces;
    }

    const QSet<QString>& readSystemEnabledPhysicalNetworkAdapterOfHardwareAddress() {
        static QSet<QString> hardwareAddress;

        if (hardwareAddress.isEmpty()) {
            QProcess process;
            process.setProcessChannelMode(QProcess::MergedChannels);
            process.start(R"(C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe)",
                          {"Get-NetAdapter | Where-Object { -not $_.Virtual -and $_.Status -eq 'Up' } | Select-Object -ExpandProperty MacAddress"});
            process.waitForFinished();
            auto data = process.readAllStandardOutput();
            auto lines = data.split('\n');
            for (const auto &line: lines) {
                if (!line.isEmpty()) {
                    auto address = QString(line).remove("\r").replace('-', ':');
                    hardwareAddress.insert(address);
                }
            }
            qInfo() << "system enabled physical network adapters:" << hardwareAddress;
        }

        return hardwareAddress;
    }

    bool InterfaceTools::testInterfaceIsPhysicalAdapter(const QNetworkInterface &i) {
        if (i.type() != QNetworkInterface::Ethernet && i.type() != QNetworkInterface::Wifi) {
            return false;
        }

        const auto& systemPhysicalAdapters = readSystemEnabledPhysicalNetworkAdapterOfHardwareAddress();
        if (!systemPhysicalAdapters.contains(i.hardwareAddress())) {
            return false;
        }

        return true;
    }
}