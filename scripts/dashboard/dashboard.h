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
#include <curl/curl.h>

#include <mutex>
#include <thread>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
using namespace rapidjson;

#include "utils.h"
#include "browse.h"
#include "vehicle.h"


using namespace std;
using namespace std::chrono;
using namespace boost::filesystem;

// To reduce data to be sent
#define MAX_DT 0.010
// Flag to enable continuous send
#define REAL_TIME true
// Timeout at which send if in mode "REAL_TIME"
#define TIMEOUT 500

Chimera chimera;
CURL *curl;

void setup_chimera_data(vector<unordered_map<string, vector<double>>> &);
void setup_json();

int setup_curl(string url);
int send_json(string url, string data);

string pack_json(string name, vector<unordered_map<string, vector<double>>>);


#endif
