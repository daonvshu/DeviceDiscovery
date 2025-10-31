project(DeviceDiscoveryDevice)

set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 17)

find_package(QT NAMES Qt5 Qt6)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network)

include(${CMAKE_SOURCE_DIR}/src/devicediscovery/comm/comm.cmake)

if (DeviceDiscovery_BUILD_STATIC)
    set(_build_type STATIC)
else ()
    set(_build_type SHARED)
endif ()

add_library(${PROJECT_NAME} ${_build_type}
    ibroadcaststrategy.h

    discoveryservice.h
    discoveryservice.cpp

    strategy/udpbroadcaststrategy.h
    strategy/udpbroadcaststrategy.cpp
    strategy/udpmulticastbroadcaststrategy.h
    strategy/udpmulticastbroadcaststrategy.cpp

    ${DeviceDiscoveryComm}
)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
)

if (DeviceDiscovery_BUILD_STATIC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DEVICE_DISCOVERY_STATIC)
else ()
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEVICE_DISCOVERY_LIBRARY)
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/comm>
    $<INSTALL_INTERFACE:include>
)

include(CMakePackageConfigHelpers)

install(TARGETS ${PROJECT_NAME}
    EXPORT DeviceDiscoveryDeviceTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/devicediscovery/device
    FILES_MATCHING PATTERN "*.h"
)

configure_package_config_file(${CMAKE_SOURCE_DIR}/cmake/DeviceDiscoveryDeviceConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/DeviceDiscoveryDeviceConfig.cmake"
    INSTALL_DESTINATION lib/cmake/DeviceDiscoveryDevice
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/DeviceDiscoveryDeviceConfig.cmake"
    DESTINATION lib/cmake/DeviceDiscoveryDevice
)

install(EXPORT DeviceDiscoveryDeviceTargets
    FILE DeviceDiscoveryDeviceTargets.cmake
    NAMESPACE DeviceDiscovery::
    DESTINATION lib/cmake/DeviceDiscoveryDevice
)