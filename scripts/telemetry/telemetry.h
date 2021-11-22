#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <ctime>
#include <chrono>
#include <stdio.h>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <iostream>
#include <exception>

#include <cstdio>
#include <ctime>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <boost/filesystem.hpp>

#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
using namespace rapidjson;

struct CAN_Stat_t
{
  double duration;
  uint64_t msg_count;
};

#include "can.h"
#include "utils.h"
#include "serial.h"
#include "vehicle.h"
#include "gps_logger.h"

#include "websocket.h"
#include "devices.pb.h"

#include "common_definitions.h"

#include "console.h"

using namespace std;
using namespace std::chrono;
using namespace boost::filesystem;

// timeout in seconds
double TIMEOUT = 0.200;
const char *CAN_DEVICE = "can0";
const char *GPS_DEVICE = "/home/gps2";

atomic<int> run_state;
bool state_changed = false;
mutex mtx;

string HOME_PATH;
string FOLDER_PATH;


int id;
uint8_t *msg_data = new uint8_t[8];

std::fstream* dump_file;

Debug::Console* console;
Client* c;
Chimera* chimera;
Can * can;
sockaddr_can addr;
// Filter and message structs
can_filter rfilter;
can_frame message;

CAN_Stat_t can_stat;
static run_config config;

vector<Device *> modifiedDevices;

void create_header(string& out);
void create_folder_name(string& out);

int open_log_folder();
int open_can_socket();
// returns 1 if start is requested
int start(const can_frame& message);

void send_status();
void send_ws_data();

void log_can(double& timestamp, can_frame& msg, std::fstream& out);
void save_stat(string folder);

void on_gps_line(int id, string line);


/**
* Gets current timestamp in seconds
*/
double get_timestamp();

/**
* Returns a string whith int expressed as Hexadecimal
* Capital letters
*
* @param num number to be converted
* @param zeros length of the final string (num = 4 => 0000A)
* return string
*/
string get_hex(int num, int zeros);


void load_config(run_config& config, string& path);
void write_config(run_config& config, string& path);




void on_message(client* cli, websocketpp::connection_hdl hdl, message_ptr msg);



#endif
