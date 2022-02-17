#ifndef DASHBOARD_H
#define DASHBOARD_H


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

#include "utils.h"
#include "browse.h"
#include "vehicle.h"
#include "websocket.h"

#include "devices.pb.h"

using namespace std;
using namespace std::chrono;
using namespace boost::filesystem;

// Timeout at which send
#define TIMEOUT 200

Chimera chimera;

// std::string uri = "ws://127.0.0.1:8080";
// std::string uri = "ws://192.168.195.1:9090";
// std::string uri = "http://eagle-telemetry-server.herokuapp.com";
std::string uri = "http://127.0.0.1:3000";


string serialized_string;

Document d;
StringBuffer sb;
Writer<StringBuffer> w(sb);
rapidjson::Document::AllocatorType &alloc = d.GetAllocator();


double get_timestamp();

#endif
