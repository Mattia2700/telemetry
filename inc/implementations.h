#ifndef __COMMON_IMPLEMETATION__
#define __COMMON_IMPLEMETATION__

#define CANLIB_TIMESTAMP
#define primary_IDS_IMPLEMENTATION
#define secondary_IDS_IMPLEMENTATION
#define primary_NETWORK_IMPLEMENTATION
#define secondary_NETWORK_IMPLEMENTATION
#define primary_MAPPING_IMPLEMENTATION
#define secondary_MAPPING_IMPLEMENTATION
#define __TELEMETRY_CONFIG_IMPLEMENTATION__
#define __MESSAGES_IMPLEMENTATION__
#include "thirdparty/can/lib/primary/c/ids.h"
#include "thirdparty/can/lib/secondary/c/ids.h"
#include "thirdparty/can/lib/primary/c/network.h"
#include "thirdparty/can/lib/secondary/c/network.h"

#include "thirdparty/can/proto/primary/cpp/mapping.h"
#include "thirdparty/can/proto/secondary/cpp/mapping.h"

#include "telemetry_config.h"
#include "messages.h"

#endif