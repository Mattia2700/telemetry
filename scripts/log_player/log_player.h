#pragma once

#include <stdio.h>
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <signal.h>

#include <cstdio>

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


std::mutex mtx;
std::condition_variable cv;

string shared_string;

double get_timestamp();
void writer(string fname);