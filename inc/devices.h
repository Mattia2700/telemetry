#pragma once

#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <fstream>
#include <iomanip>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
using namespace rapidjson;


#include "devices.pb.h"
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/json_util.h>
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
	"GPS SPS MODE FIX VALID",
	"DIFFERENTIAL GPS SPS MODE, FIX VALID",
	"UNSUPPORTED","UNSUPPORTED","UNSUPPORTED",
	"DEAD RECKONING MODE FIX VALID"
};
static string FIX_MODE[3] = 
{
	"FIX NOT AVAILABLE",
	"2D",
	"3D"
};

class Device {
public:
	Device(std::string name = "default");

	int    get_id()  { return id; }
	std::string get_name(){ return name; }

	virtual std::string get_header(std::string separator) = 0;
	virtual std::string get_string(std::string separator) = 0;
	virtual Document    get_json  () = 0;
	virtual std::string   get_readable() = 0;

	std::string json_string();
	void serialized_to_string(Message* message, std::string *out);
	void serialized_to_text(Message* message, std::string *out);
	void serialized_to_json(Message* message, std::string *out);
	static std::vector<std::string> get_field_names(const Descriptor* descriptor);
	static string get_header(const std::vector<std::string>& fields, string separator);
	static string get_header(const Descriptor* descriptor, std::string separator);

	std::vector<std::string>   filenames;
	std::vector<std::fstream*> files;

	long int samples_count;
	double prev_timestamp;

private:
	int id;
	std::string name;
	static int instance_count;
};

class Imu: public Device{
public:
	Imu(std::string name): Device(name){};

	virtual std::string get_header(std::string separator);
	virtual std::string get_string(std::string separator);
	virtual Document    get_json  ();
	virtual std::string   get_readable();

	void serialize(devices::Imu* device);

	double timestamp;
	double x;
	double y;
	double z;
	double scale;
};

class Encoder: public Device{
public:
	Encoder(std::string name): Device(name){};

	virtual std::string get_header(std::string separator);
	virtual std::string get_string(std::string separator);
	virtual Document    get_json  ();
	virtual std::string   get_readable();

	void serialize(devices::Encoder* device);

	double timestamp;
	double rads;
	double km;
	double rotations;
};

class Steer: public Device{
public:
	Steer(std::string name): Device(name){};

	virtual std::string get_header(std::string separator);
	virtual std::string get_string(std::string separator);
	virtual Document    get_json  ();
	virtual std::string   get_readable();

	void serialize(devices::Steer* device);

	double timestamp;
	double angle;
};

class Pedals: public Device{
public:
	Pedals(std::string name): Device(name){};

	virtual std::string get_header(std::string separator);
	virtual std::string get_string(std::string separator);
	virtual Document    get_json  ();
	virtual std::string   get_readable();

	void serialize(devices::Pedals* device);

	double timestamp;
	double throttle1;
	double throttle2;
	double brake_front;
	double brake_rear;
};

class Inverter: public Device{
public:
	Inverter(std::string name): Device(name){};

	virtual std::string get_header(std::string separator);
	virtual std::string get_string(std::string separator);
	virtual Document    get_json  ();
	virtual std::string   get_readable();

	void serialize(devices::Inverter* device);

	double timestamp;
	double temperature;
	double motor_temp;
	double torque;
	double speed;
};

class Bms: public Device{
public:
	Bms(std::string name): Device(name){};

	virtual std::string get_header(std::string separator);
	virtual std::string get_string(std::string separator);
	virtual Document    get_json  ();
	virtual std::string   get_readable();

	void serialize(devices::Bms* device);

	double timestamp;
	double temperature;
	double max_temperature;
	double min_temperature;
	double current;
	double voltage;
	double max_voltage;
	double min_voltage;
	double power;
};

class Ecu: public Device{
public:
	Ecu(std::string name): Device(name){};

	virtual std::string get_header(std::string separator);
	virtual std::string get_string(std::string separator);
	virtual Document    get_json  ();
	virtual std::string   get_readable();

	void serialize(devices::Ecu* device);

	double timestamp;
	double power_request_left;
	double power_request_right;
};

class State: public Device{
public:
	State(std::string name): Device(name){};

	virtual std::string get_header(std::string separator);
	virtual std::string get_string(std::string separator);
	virtual Document    get_json  ();
	virtual std::string   get_readable();

	void serialize(devices::State* device);

	double timestamp;
	std::string value;
};

class Temperature: public Device{
	public:
	Temperature(std::string name): Device(name){};

	virtual std::string get_header(std::string separator);
	virtual std::string get_string(std::string separator);
	virtual Document    get_json  ();
	virtual std::string   get_readable();

	void serialize(devices::Temperature* device);

	double timestamp;
	double temps[16];	// 16 channels sensor
};

class Gps: public Device{
public:
	Gps(std::string name): Device(name){};

	virtual std::string get_header(std::string separator);
	virtual std::string get_string(std::string separator);
	virtual Document    get_json  ();
	virtual std::string   get_readable();

	void serialize(devices::Gps* device);

	void clear(){
		timestamp = 0.0;
		msg_type = "";
		time = "";
		latitude = 0.0;
		longitude = 0.0;
		altitude = 0.0;
		fix = 0;
		satellites = 0;
		fix_state = "";
		age_of_correction = 0.0;
		course_over_ground_degrees = 0.0;
		course_over_ground_degrees_magnetic = 0.0;
		speed_kmh = 0.0;
		mode = "";
		position_diluition_precision = 0.0;
		horizontal_diluition_precision = 0.0;
		vertical_diluition_precision = 0.0;
	}

	double timestamp;
	std::string msg_type;
	std::string time;
	double latitude;
	double longitude;
	double altitude;
	uint32_t fix;
	uint32_t satellites;
	std::string fix_state;
	double age_of_correction;
	double course_over_ground_degrees;
	double course_over_ground_degrees_magnetic;
	double speed_kmh;
	std::string mode;
	double position_diluition_precision;
	double horizontal_diluition_precision;
	double vertical_diluition_precision;
};
