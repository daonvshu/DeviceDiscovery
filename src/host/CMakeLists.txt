project(DeviceDiscoveryHost)

set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 17)

find_package(QT NAMES Qt5 Qt6)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network)

include(${CMAKE_SOURCE_DIR}/src/comm/comm.cmake)

set(CMAKE_DEBUG_POSTFIX "d")

add_library(${PROJECT_NAME} STATIC
    idiscoverystrategy.h

    discoverymanager.h
    discoverymanager.cpp

    interfacetools.h
    interfacetools.cpp

    strategy/udpdiscoverystrategy.h
    strategy/udpdiscoverystrategy.cpp
    strategy/udpmulticastdiscoverystrategy.h
    strategy/udpmulticastdiscoverystrategy.cpp

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
    EXPORT DeviceDiscoveryHostTargets
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

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/DeviceDiscoveryHostConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/DeviceDiscoveryHostConfig.cmake"
    INSTALL_DESTINATION lib/cmake/DeviceDiscoveryHost
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/DeviceDiscoveryHostConfig.cmake"
    DESTINATION lib/cmake/DeviceDiscoveryHost
)

install(EXPORT DeviceDiscoveryHostTargets
    FILE DeviceDiscoveryHostTargets.cmake
    NAMESPACE DeviceDiscovery::
    DESTINATION lib/cmake/DeviceDiscoveryHost
)