#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <iostream>

#include <boost/filesystem.hpp>

#include "can.h"

using namespace std;
using namespace boost::filesystem;

string HOME_PATH;
string FOLDER_PATH;

int id;
uint8_t* data = new uint8_t[8];
can_frame message;

int main();

string get_last_fname(string path);

#endif