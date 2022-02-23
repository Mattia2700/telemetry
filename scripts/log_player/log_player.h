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

#include <filesystem>

#include <mutex>
#include <thread>

#include "can.h"
#include "utils.h"
#include "browse.h"

using namespace std;
using namespace std::chrono;
using namespace filesystem;

string serialized_string;

const char *CAN_DEVICE;
sockaddr_can addr;
Can * can;

double get_timestamp();

#endif
