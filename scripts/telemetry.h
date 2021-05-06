#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <chrono>
#include <stdio.h>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <iostream>

#include <boost/filesystem.hpp>

#include "can.h"

using namespace std;
using namespace boost::filesystem;
using namespace std::chrono;

const char* CAN_DEVICE = "vcan0";
string HOME_PATH;
string FOLDER_PATH;

vector<string> CIRCUITS;
vector<string> PILOTS;
vector<string> RACES;

int id;
uint8_t* data = new uint8_t[8];
can_frame message;

int main();

string get_last_fname(string path);
double get_timestamp();

string get_hex(int num, int zeros);

#endif