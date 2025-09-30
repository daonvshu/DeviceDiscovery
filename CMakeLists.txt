cmake_minimum_required(VERSION 3.15)

project(DeviceDiscovery)

if (WIN32)
add_subdirectory(src/host)
else ()
add_subdirectory(src/device)
endif ()

add_subdirectory(test)