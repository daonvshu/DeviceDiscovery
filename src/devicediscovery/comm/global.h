#pragma once

#include <QtCore/QtGlobal>

#define DEVICE_DISCOVERY_BEGIN_NAMESPACE namespace DeviceDiscovery {
#define DEVICE_DISCOVERY_END_NAMESPACE }
#define DEVICE_DISCOVERY_USE_NAMESPACE using namespace DeviceDiscovery;

#ifndef DEVICE_DISCOVERY_EXPORT
#  ifdef DEVICE_DISCOVERY_STATIC
#    define DEVICE_DISCOVERY_EXPORT
#  else
#    ifdef DEVICE_DISCOVERY_LIBRARY
#      define DEVICE_DISCOVERY_EXPORT Q_DECL_EXPORT
#    else
#      define DEVICE_DISCOVERY_EXPORT Q_DECL_IMPORT
#    endif
#  endif
#endif