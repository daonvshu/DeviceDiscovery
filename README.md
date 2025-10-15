# DeviceDiscovery
设备扫描库，可以配置多种扫描策略发现设备。

## 设备端配置

```c++
#include <discoveryservice.h>
#include <strategy/udpbroadcaststrategy.h>
#include <strategy/udpmulticastbroadcaststrategy.h>

// 添加广播扫描策略
DiscoveryService::addStrategy(
    UdpBroadCastStrategyBuilder()
        .listenOn(19288) // 监听端口
        .bindEth()  // 绑定eth网卡
        .bindUsb()  // 绑定usb网卡
        .bindWifi() // 绑定wifi网卡
        .bindInterface("^wlp") // 绑定其他网卡
        .build()
);

// 添加组播扫描策略
DiscoveryService::addStrategy(
    UdpMulticastStrategyBuilder()
        .bindAddress("239.0.1.1") // 组播地址
        .listenOn(19288)
        .bindEth()
        .bindUsb()
        .bindWifi()
        .bindInterface("^wlp")
        .build()
);

// 设置签名key，签名不一致将忽略响应
DiscoveryService::setStrategiesKey("random_key");
// 启动服务
DiscoveryService::start();
```

## 管理端配置

```c++
#include <discoverymanager.h>
#include <strategy/udpbroadcastdiscoverystrategy.h>
#include <strategy/udpmulticastdiscoverystrategy.h>

// 添加广播扫描策略
DiscoveryManager::addStrategy(
    UdpBroadcastStrategyBuilder()
        .notifyOn(19288) // 扫描端口
        .build()
);

// 添加组播扫描策略
DiscoveryManager::addStrategy(
    UdpMulticastStrategyBuilder()
        .bindAddress("239.0.1.1") // 组播地址
        .notifyOn(19288) // 扫描端口
        .build()
);

// 设置签名key，签名不一致将忽略响应
DiscoveryManager::setStrategiesKey("random_key");
```

扫描设备

```c++
#include <discoverymanager.h>

// 启动扫描，3s后停止并回调扫描结果
DiscoveryManager::startScan(3000, [] (const QList<DeviceRecord>& records) {
    qInfo() << "get record size:" << records.size();
});
```