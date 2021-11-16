#ifndef DEVICES_H
#define DEVICES_H

#include <ctime>
#include <chrono>
#include <stdio.h>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <iostream>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
using namespace rapidjson;

#include "devices.pb.h"
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/json_util.h>

using namespace std;
using namespace std::chrono;
using namespace google::protobuf;
using namespace google::protobuf::util;

enum ECU_STATE_T
{
	IDLE,
	SETUP,
	RUN,
  NUM_STATES
};
enum BMS_WARNING_T
{
	WARN_CELL_LOW_VOLTAGE,
	WARN_CELL_DROPPING,
	WARN_PRECHARGE_FAIL,
  NUM_WARNINGS
};
enum BMS_ERROR_T
{
	ERROR_LTC6804_PEC_ERROR,
	ERROR_CELL_UNDER_VOLTAGE,
	ERROR_CELL_OVER_VOLTAGE,
	ERROR_CELL_OVER_TEMPERATURE,
	ERROR_OVER_CURRENT,
	ERROR_CAN,
  NUM_ERRORS
};
static string ECU_STATES[NUM_STATES] =
{
  "IDLE",
  "SETUP",
  "RUN"
};
static string BMS_WARNINGS[NUM_WARNINGS] =
{
  "WARN_CELL_LOW_VOLTAGE",
  "WARN_CELL_DROPPING",
  "WARN_PRECHARGE_FAIL",
};
static string BMS_ERRORS[NUM_ERRORS] =
{
  "ERROR_LTC6804_PEC_ERROR",
  "ERROR_CELL_UNDER_VOLTAGE",
  "ERROR_CELL_OVER_VOLTAGE",
  "ERROR_CELL_OVER_TEMPERATURE",
  "ERROR_OVER_CURRENT",
  "ERROR_CAN"
};

static string FIX_STATE[7] =
{
	"FIX NOT AVAILABLE OR INVALID",
	"GPS SPS MODE, FIX VALID",
	"DIFFERENTIAL GPS SPS MODE, FIX VALID",
	"UNSUPPORTED","UNSUPPORTED","UNSUPPORTED",
	"DEAD RECKONING MODE, FIX VALID"
};

class Device
{
public:
  Device(string name = "default");
  virtual ~Device();

  int get_id() { return id; };
  string get_name() { return name; };

  virtual string get_header(string separator) = 0;
  virtual string get_string(string separator) = 0;
  virtual Document json() = 0;

  virtual string json_string()
  {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    json().Accept(writer);

    return buffer.GetString();
  }

  static vector<string> get_field_names(const Descriptor* descriptor);
  static string get_header(const vector<string>& fields, string separator);
  static string get_header(const Descriptor* descriptor, string separator);


  /**
  * From protobuf object to string serialized
  *
  * @param out serialized string in output
  */
  void serialized_to_string(Message* message, string *out){
    message->SerializeToString(out);
  }

  /**
  * Human readable string of the serialized object
  *
  * @param outout string
  */
  void serialized_to_text(Message* message, string *out){
    TextFormat::PrintToString(*message, out);
  }
  /**
  * JSON readable string of the serialized object
  *
  * @param outout string
  */
  void serialized_to_json(Message* message, string *out){
    MessageToJsonString(*message, out);
  }

  double timestamp;

  int count;
  float helper_variable = 0;
  vector<std::fstream *> files;
  vector<string> filenames;
  vector<string> mesages_list;

private:
  string name;

  int id;
  static int global_id;
};

class Imu : public Device
{
public:
  Imu(string name = "default") : Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
  virtual Document json();

  void serialize(devices::Imu *);

  float x;
  float y;
  float z;
  float scale;
};

class Encoder : public Device
{
public:
  Encoder(string name = "default") : Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
  virtual Document json();

  void serialize(devices::Encoder *);

  float rads;
  float km;
  float rotations;
};

class Steer : public Device
{
public:
  Steer(string name = "default") : Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
  virtual Document json();

  void serialize(devices::Steer *);

  float angle;
};

class Pedals : public Device
{
public:
  Pedals(string name = "default") : Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
  virtual Document json();

  void serialize(devices::Pedals *);

  float throttle1;
  float throttle2;
  float brake_front;
  float brake_rear;
};

class Inverter : public Device
{
public:
  Inverter(string name = "default") : Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
  virtual Document json();

  void serialize(devices::Inverter *);

  float temperature;
  float motor_temp;
  float torque;
  float speed;
};

class Bms : public Device
{
public:
  Bms(string name = "default") : Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
  virtual Document json();

  void serialize(devices::Bms *);

  float temperature;
  float max_temperature;
  float min_temperature;

  float current;

  float voltage;
  float max_voltage;
  float min_voltage;

  float power;
};

class State : public Device
{
public:
  State(string name = "default") : Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
  virtual Document json();

  void serialize(devices::State *);

  string value;
};

class Ecu : public Device
{
public:
  Ecu(string name = "default") : Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
  virtual Document json();

  void serialize(devices::Ecu *);

  double power_request_left;
  double power_request_right;
};

class GPS : public Device
{
public:
  GPS(string name = "default") : Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
  virtual Document json();

  void serialize(devices::GPS *);

	// if GGA or VTG or ...
	string msg_type;

	// From GGA string
	string time;
	double latitude;
	double longitude;
	int fix;
	int satellites;
	string fix_state;
	double altitude;
	double age_of_correction;

	// From VTG string
	double course_over_ground_degrees;
	double course_over_ground_degrees_magnetic;
	double speed_kmh;
	string mode;
};

#endif //DEVICES_H
