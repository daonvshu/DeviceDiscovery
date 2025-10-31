cmake_minimum_required(VERSION 3.15)

project(DeviceDiscovery VERSION 1.0.0)

add_subdirectory(src/devicediscovery/host)
add_subdirectory(src/devicediscovery/device)

option(DeviceDiscovery_EnableTest "Enable testing" OFF)
option(DeviceDiscovery_BUILD_STATIC "Build static libraries" ON)

if (DeviceDiscovery_EnableTest)
    add_subdirectory(test)
endif ()

include(CMakePackageConfigHelpers)

add_custom_target(DeviceDiscovery ALL)
add_dependencies(DeviceDiscovery
    DeviceDiscoveryDevice
    DeviceDiscoveryHost
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/DeviceDiscoveryConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_SOURCE_DIR}/cmake/DeviceDiscoveryConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/DeviceDiscoveryConfig.cmake"
    INSTALL_DESTINATION lib/cmake/DeviceDiscovery
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/devicediscovery/comm/
    DESTINATION include/devicediscovery/comm
    FILES_MATCHING PATTERN "*.h"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/DeviceDiscoveryConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/DeviceDiscoveryConfigVersion.cmake"
    DESTINATION lib/cmake/DeviceDiscovery
)
