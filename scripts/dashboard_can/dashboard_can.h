#ifndef DASHBOARD_CAN_H
#define DASHBOARD_CAN_H


#include <ctime>
#include <chrono>
#include <stdio.h>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <iostream>

#include <boost/filesystem.hpp>

#include <mutex>
#include <thread>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
using namespace rapidjson;

#include "can.h"
#include "utils.h"
#include "utils.h"
#include "vehicle.h"
#include "websocket.h"

#include "devices.pb.h"

using namespace std;
using namespace std::chrono;
using namespace boost::filesystem;

// To reduce data to be sent
#define MAX_DT 0.010
// Flag to enable continuous send
#define REAL_TIME true
// Timeout at which send if in mode "REAL_TIME"
#define TIMEOUT 200

Chimera chimera;

const char* CAN_DEVICE = "vcan0";

string uri = "ws://192.168.195.3:8080";

string serialized_string;

Document d;
StringBuffer sb;
Writer<StringBuffer> w(sb);
rapidjson::Document::AllocatorType &alloc = d.GetAllocator();

double get_timestamp();


#endif
