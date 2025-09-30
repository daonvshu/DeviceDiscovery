project(DeviceDiscovery-Test)

set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 17)

find_package(QT NAMES Qt5 Qt6)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} DeviceDiscoveryHost)
else ()
    target_link_libraries(${PROJECT_NAME} DeviceDiscoveryDevice)
endif ()