#pragma once

#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <list>
#include <fstream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
using namespace rapidjson;


#include "devices.pb.h"
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/json_util.h>
using namespace google::protobuf;
using namespace google::protobuf::util;


template<typename T>
void Erase(T& list, size_t maxSize)
{
	size_t size = list.size();
	if(size > maxSize)
	{
		auto bgn = list.begin();
		auto end = list.begin();
		std::advance(end, size - maxSize);
		list.erase(bgn, end);
	}
}

class Imu{
public:
	std::list<double> timestamp;
	std::list<double> x;
	std::list<double> y;
	std::list<double> z;
	std::list<double> scale;
	void CheckFieldsSize(uint64_t maxSize);
};

class Encoder{
public:
	std::list<double> timestamp;
	std::list<double> rads;
	std::list<double> km;
	std::list<double> rotations;
	void CheckFieldsSize(uint64_t maxSize);
};

class Steer{
public:
	std::list<double> timestamp;
	std::list<double> angle;
	void CheckFieldsSize(uint64_t maxSize);
};

class Pedals{
public:
	std::list<double> timestamp;
	std::list<double> throttle1;
	std::list<double> throttle2;
	std::list<double> brake_front;
	std::list<double> brake_rear;
	void CheckFieldsSize(uint64_t maxSize);
};

class Inverter{
public:
	std::list<double> timestamp;
	std::list<double> temperature;
	std::list<double> motor_temp;
	std::list<double> torque;
	std::list<double> speed;
	void CheckFieldsSize(uint64_t maxSize);
};

class Bms{
public:
	std::list<double> timestamp;
	std::list<double> temperature;
	std::list<double> max_temperature;
	std::list<double> min_temperature;
	std::list<double> current;
	std::list<double> voltage;
	std::list<double> max_voltage;
	std::list<double> min_voltage;
	std::list<double> power;
	void CheckFieldsSize(uint64_t maxSize);
};

class Ecu{
public:
	std::list<double> timestamp;
	std::list<double> power_request_left;
	std::list<double> power_request_right;
	void CheckFieldsSize(uint64_t maxSize);
};

class State{
public:
	std::list<double> timestamp;
	std::list<std::string> value;
	void CheckFieldsSize(uint64_t maxSize);
};

class Gps{
public:
	std::list<double> timestamp;
	std::list<std::string> msg_type;
	std::list<std::string> time;
	std::list<double> latitude;
	std::list<double> longitude;
	std::list<double> altitude;
	std::list<uint32_t> fix;
	std::list<uint32_t> satellites;
	std::list<std::string> fix_state;
	std::list<double> age_of_correction;
	std::list<double> course_over_ground_degrees;
	std::list<double> course_over_ground_degrees_magnetic;
	std::list<double> speed_kmh;
	std::list<std::string> mode;
	void CheckFieldsSize(uint64_t maxSize);
};

class ChimeraDeserializer{
public:
	ChimeraDeserializer(){};

	void Deserialize(devices::Chimera* device);

	Imu accel;
	Imu gyro;
	Encoder encoder_left;
	Encoder encoder_right;
	Bms bms_lv;
	Bms bms_hv;
	Inverter inverter_left;
	Inverter inverter_right;
	Pedals pedal;
	Steer steer;
	State ecu_state;
	State bms_hv_state;
	State steering_wheel_state;
	Ecu ecu;
	Gps gps1;
	Gps gps2;

	void SetMaxDeviceSize(uint64_t size){m_MaxDeviceSize = size;};
private:
	uint64_t m_MaxDeviceSize;

};
