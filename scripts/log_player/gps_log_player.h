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

#include "utils.h"
#include "browse.h"

using namespace std;
using namespace std::chrono;
using namespace filesystem;

string serialized_string;

std::mutex mtx;
std::condition_variable cv;

string shared_string;

void writer(string fname);