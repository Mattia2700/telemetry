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
#include <curl/curl.h>

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
CURL *curl;

const char* CAN_DEVICE = "vcan0";

string serialized_string;

int setup_curl(string url);
int send_text(string url, string data);
double get_timestamp();


#endif
