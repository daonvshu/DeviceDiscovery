project(DeviceDiscoveryDevice VERSION 1.0.0)

set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 17)

find_package(QT NAMES Qt5 Qt6)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network)

include(${CMAKE_SOURCE_DIR}/src/comm/comm.cmake)

add_library(${PROJECT_NAME} STATIC
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

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/comm>
    $<INSTALL_INTERFACE:include>
    $<INSTALL_INTERFACE:include>/comm
)

include(CMakePackageConfigHelpers)

install(TARGETS ${PROJECT_NAME}
    EXPORT DeviceDiscoveryDeviceTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/comm/
    DESTINATION include/comm
    FILES_MATCHING PATTERN "*.h"
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/DeviceDiscoveryDeviceConfig.cmake.in
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