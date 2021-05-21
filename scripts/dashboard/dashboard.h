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

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "utils.h"
#include "browse.h"
#include "vehicle.h"


using namespace std;
using namespace std::chrono;
using namespace boost::filesystem;

#define TIMEOUT 1000


#endif
