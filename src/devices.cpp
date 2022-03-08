#include "devices.h"

int Device::instance_count = 0;


Device::Device(std::string name)
{
	id = instance_count;
	instance_count ++;
	this->name = name;
	prev_timestamp = 0.0;
}
std::string Device::json_string()
{
	StringBuffer sb;
	Writer<StringBuffer> writer(sb);
	get_json().Accept(writer);
	return sb.GetString();
}
std::vector<std::string> Device::get_field_names(const Descriptor* descriptor){
	std::vector<std::string> fields;
	for(int i = 0; i < descriptor->field_count(); i++){
		fields.push_back(descriptor->field(i)->json_name());
	}
	return fields;
}
std::string Device::get_header(const std::vector<std::string>& fields, std::string separator){
	std::string header = "";
	for(int i = 0; i < fields.size()-1; i++){
		header += fields[i] + separator;
	}
	header += fields[fields.size()-1];
	return header;
}
std::string Device::get_header(const Descriptor* descriptor, std::string separator){
	std::string header = "";
	int count = descriptor->field_count();
	for(int i = 0; i < count-1; i++){
		header += descriptor->field(i)->json_name() + separator;
	}
	header += descriptor->field(count-1)->json_name();
	return header;
}

///////////////////////////////
////////////  IMU  ////////////
///////////////////////////////
std::string Imu::get_header(std::string separator)
{
	std::string header = "";
	header += "timestamp" + separator;
	header += "x" + separator;
	header += "y" + separator;
	header += "z" + separator;
	header += "scale" + separator;
	return header;
}
std::string Imu::get_string(std::string separator)
{
	std::string str = "";
	str += std::to_string(timestamp) + separator;
	str += std::to_string(x) + separator;
	str += std::to_string(y) + separator;
	str += std::to_string(z) + separator;
	str += std::to_string(scale) + separator;
	return str;
}
Document Imu::get_json()
{
	Document d;
	d.SetObject();
	Document::AllocatorType &alloc = d.GetAllocator();
	d.AddMember("timestamp", timestamp, alloc);
	d.AddMember("x", x, alloc);
	d.AddMember("y", y, alloc);
	d.AddMember("z", z, alloc);
	d.AddMember("scale", scale, alloc);
	return d;
}
void Imu::serialize(devices::Imu* imu)
{
	imu->set_timestamp(timestamp);
	imu->set_x(x);
	imu->set_y(y);
	imu->set_z(z);
	imu->set_scale(scale);
}
std::string Imu::get_readable()
{
	std::stringstream ss;
	ss.precision(3);
	ss << get_name() << "\n";
	ss << "\ttimestamp -> \t" << timestamp << "\n";
	ss << "\tx -> \t" << x << "\n";
	ss << "\ty -> \t" << y << "\n";
	ss << "\tz -> \t" << z << "\n";
	ss << "\tscale -> \t" << scale << "\n";
	return ss.str();
}

///////////////////////////////
//////////  ENCODER  //////////
///////////////////////////////
std::string Encoder::get_header(std::string separator)
{
	std::string header = "";
	header += "timestamp" + separator;
	header += "rads" + separator;
	header += "km" + separator;
	header += "rotations" + separator;
	return header;
}
std::string Encoder::get_string(std::string separator)
{
	std::string str = "";
	str += std::to_string(timestamp) + separator;
	str += std::to_string(rads) + separator;
	str += std::to_string(km) + separator;
	str += std::to_string(rotations) + separator;
	return str;
}
Document Encoder::get_json()
{
	Document d;
	d.SetObject();
	Document::AllocatorType &alloc = d.GetAllocator();
	d.AddMember("timestamp", timestamp, alloc);
	d.AddMember("rads", rads, alloc);
	d.AddMember("km", km, alloc);
	d.AddMember("rotations", rotations, alloc);
	return d;
}
void Encoder::serialize(devices::Encoder* encoder)
{
	encoder->set_timestamp(timestamp);
	encoder->set_rads(rads);
	encoder->set_km(km);
	encoder->set_rotations(rotations);
}
std::string Encoder::get_readable()
{
	std::stringstream ss;
	ss.precision(3);
	ss << get_name() << "\n";
	ss << "\ttimestamp -> \t" << timestamp << "\n";
	ss << "\trads -> \t" << rads << "\n";
	ss << "\tkm -> \t" << km << "\n";
	ss << "\trotations -> \t" << rotations << "\n";
	return ss.str();
}

///////////////////////////////
///////////  STEER  ///////////
///////////////////////////////
std::string Steer::get_header(std::string separator)
{
	std::string header = "";
	header += "timestamp" + separator;
	header += "angle" + separator;
	return header;
}
std::string Steer::get_string(std::string separator)
{
	std::string str = "";
	str += std::to_string(timestamp) + separator;
	str += std::to_string(angle) + separator;
	return str;
}
Document Steer::get_json()
{
	Document d;
	d.SetObject();
	Document::AllocatorType &alloc = d.GetAllocator();
	d.AddMember("timestamp", timestamp, alloc);
	d.AddMember("angle", angle, alloc);
	return d;
}
void Steer::serialize(devices::Steer* steer)
{
	steer->set_timestamp(timestamp);
	steer->set_angle(angle);
}
std::string Steer::get_readable()
{
	std::stringstream ss;
	ss.precision(3);
	ss << get_name() << "\n";
	ss << "\ttimestamp -> \t" << timestamp << "\n";
	ss << "\tangle -> \t" << angle << "\n";
	return ss.str();
}

///////////////////////////////
//////////  PEDALS  //////////
///////////////////////////////
std::string Pedals::get_header(std::string separator)
{
	std::string header = "";
	header += "timestamp" + separator;
	header += "throttle1" + separator;
	header += "throttle2" + separator;
	header += "brake_front" + separator;
	header += "brake_rear" + separator;
	return header;
}
std::string Pedals::get_string(std::string separator)
{
	std::string str = "";
	str += std::to_string(timestamp) + separator;
	str += std::to_string(throttle1) + separator;
	str += std::to_string(throttle2) + separator;
	str += std::to_string(brake_front) + separator;
	str += std::to_string(brake_rear) + separator;
	return str;
}
Document Pedals::get_json()
{
	Document d;
	d.SetObject();
	Document::AllocatorType &alloc = d.GetAllocator();
	d.AddMember("timestamp", timestamp, alloc);
	d.AddMember("throttle1", throttle1, alloc);
	d.AddMember("throttle2", throttle2, alloc);
	d.AddMember("brake_front", brake_front, alloc);
	d.AddMember("brake_rear", brake_rear, alloc);
	return d;
}
void Pedals::serialize(devices::Pedals* pedals)
{
	pedals->set_timestamp(timestamp);
	pedals->set_throttle1(throttle1);
	pedals->set_throttle2(throttle2);
	pedals->set_brake_front(brake_front);
	pedals->set_brake_rear(brake_rear);
}
std::string Pedals::get_readable()
{
	std::stringstream ss;
	ss.precision(3);
	ss << get_name() << "\n";
	ss << "\ttimestamp -> \t" << timestamp << "\n";
	ss << "\tthrottle1 -> \t" << throttle1 << "\n";
	ss << "\tthrottle2 -> \t" << throttle2 << "\n";
	ss << "\tbrake_front -> \t" << brake_front << "\n";
	ss << "\tbrake_rear -> \t" << brake_rear << "\n";
	return ss.str();
}

///////////////////////////////
/////////  INVERTER  /////////
///////////////////////////////
std::string Inverter::get_header(std::string separator)
{
	std::string header = "";
	header += "timestamp" + separator;
	header += "temperature" + separator;
	header += "motor_temp" + separator;
	header += "torque" + separator;
	header += "speed" + separator;
	return header;
}
std::string Inverter::get_string(std::string separator)
{
	std::string str = "";
	str += std::to_string(timestamp) + separator;
	str += std::to_string(temperature) + separator;
	str += std::to_string(motor_temp) + separator;
	str += std::to_string(torque) + separator;
	str += std::to_string(speed) + separator;
	return str;
}
Document Inverter::get_json()
{
	Document d;
	d.SetObject();
	Document::AllocatorType &alloc = d.GetAllocator();
	d.AddMember("timestamp", timestamp, alloc);
	d.AddMember("temperature", temperature, alloc);
	d.AddMember("motor_temp", motor_temp, alloc);
	d.AddMember("torque", torque, alloc);
	d.AddMember("speed", speed, alloc);
	return d;
}
void Inverter::serialize(devices::Inverter* inverter)
{
	inverter->set_timestamp(timestamp);
	inverter->set_temperature(temperature);
	inverter->set_motor_temp(motor_temp);
	inverter->set_torque(torque);
	inverter->set_speed(speed);
}
std::string Inverter::get_readable()
{
	std::stringstream ss;
	ss.precision(3);
	ss << get_name() << "\n";
	ss << "\ttimestamp -> \t" << timestamp << "\n";
	ss << "\ttemperature -> \t" << temperature << "\n";
	ss << "\tmotor_temp -> \t" << motor_temp << "\n";
	ss << "\ttorque -> \t" << torque << "\n";
	ss << "\tspeed -> \t" << speed << "\n";
	return ss.str();
}

///////////////////////////////
////////////  BMS  ////////////
///////////////////////////////
std::string Bms::get_header(std::string separator)
{
	std::string header = "";
	header += "timestamp" + separator;
	header += "temperature" + separator;
	header += "max_temperature" + separator;
	header += "min_temperature" + separator;
	header += "current" + separator;
	header += "voltage" + separator;
	header += "max_voltage" + separator;
	header += "min_voltage" + separator;
	header += "power" + separator;
	return header;
}
std::string Bms::get_string(std::string separator)
{
	std::string str = "";
	str += std::to_string(timestamp) + separator;
	str += std::to_string(temperature) + separator;
	str += std::to_string(max_temperature) + separator;
	str += std::to_string(min_temperature) + separator;
	str += std::to_string(current) + separator;
	str += std::to_string(voltage) + separator;
	str += std::to_string(max_voltage) + separator;
	str += std::to_string(min_voltage) + separator;
	str += std::to_string(power) + separator;
	return str;
}
Document Bms::get_json()
{
	Document d;
	d.SetObject();
	Document::AllocatorType &alloc = d.GetAllocator();
	d.AddMember("timestamp", timestamp, alloc);
	d.AddMember("temperature", temperature, alloc);
	d.AddMember("max_temperature", max_temperature, alloc);
	d.AddMember("min_temperature", min_temperature, alloc);
	d.AddMember("current", current, alloc);
	d.AddMember("voltage", voltage, alloc);
	d.AddMember("max_voltage", max_voltage, alloc);
	d.AddMember("min_voltage", min_voltage, alloc);
	d.AddMember("power", power, alloc);
	return d;
}
void Bms::serialize(devices::Bms* bms)
{
	bms->set_timestamp(timestamp);
	bms->set_temperature(temperature);
	bms->set_max_temperature(max_temperature);
	bms->set_min_temperature(min_temperature);
	bms->set_current(current);
	bms->set_voltage(voltage);
	bms->set_max_voltage(max_voltage);
	bms->set_min_voltage(min_voltage);
	bms->set_power(power);
}
std::string Bms::get_readable()
{
	std::stringstream ss;
	ss.precision(3);
	ss << get_name() << "\n";
	ss << "\ttimestamp -> \t" << timestamp << "\n";
	ss << "\ttemperature -> \t" << temperature << "\n";
	ss << "\tmax_temperature -> \t" << max_temperature << "\n";
	ss << "\tmin_temperature -> \t" << min_temperature << "\n";
	ss << "\tcurrent -> \t" << current << "\n";
	ss << "\tvoltage -> \t" << voltage << "\n";
	ss << "\tmax_voltage -> \t" << max_voltage << "\n";
	ss << "\tmin_voltage -> \t" << min_voltage << "\n";
	ss << "\tpower -> \t" << power << "\n";
	return ss.str();
}

///////////////////////////////
////////////  ECU  ////////////
///////////////////////////////
std::string Ecu::get_header(std::string separator)
{
	std::string header = "";
	header += "timestamp" + separator;
	header += "power_request_left" + separator;
	header += "power_request_right" + separator;
	return header;
}
std::string Ecu::get_string(std::string separator)
{
	std::string str = "";
	str += std::to_string(timestamp) + separator;
	str += std::to_string(power_request_left) + separator;
	str += std::to_string(power_request_right) + separator;
	return str;
}
Document Ecu::get_json()
{
	Document d;
	d.SetObject();
	Document::AllocatorType &alloc = d.GetAllocator();
	d.AddMember("timestamp", timestamp, alloc);
	d.AddMember("power_request_left", power_request_left, alloc);
	d.AddMember("power_request_right", power_request_right, alloc);
	return d;
}
void Ecu::serialize(devices::Ecu* ecu)
{
	ecu->set_timestamp(timestamp);
	ecu->set_power_request_left(power_request_left);
	ecu->set_power_request_right(power_request_right);
}
std::string Ecu::get_readable()
{
	std::stringstream ss;
	ss.precision(3);
	ss << get_name() << "\n";
	ss << "\ttimestamp -> \t" << timestamp << "\n";
	ss << "\tpower_request_left -> \t" << power_request_left << "\n";
	ss << "\tpower_request_right -> \t" << power_request_right << "\n";
	return ss.str();
}

///////////////////////////////
///////////  STATE  ///////////
///////////////////////////////
std::string State::get_header(std::string separator)
{
	std::string header = "";
	header += "timestamp" + separator;
	header += "value" + separator;
	return header;
}
std::string State::get_string(std::string separator)
{
	std::string str = "";
	str += std::to_string(timestamp) + separator;
	str += value + separator;
	return str;
}
Document State::get_json()
{
	Document d;
	d.SetObject();
	Document::AllocatorType &alloc = d.GetAllocator();
	d.AddMember("timestamp", timestamp, alloc);
	d.AddMember("value", Value().SetString(value.c_str(), value.size(), alloc), alloc);
	return d;
}
void State::serialize(devices::State* state)
{
	state->set_timestamp(timestamp);
	state->set_value(value);
}
std::string State::get_readable()
{
	std::stringstream ss;
	ss.precision(3);
	ss << get_name() << "\n";
	ss << "\ttimestamp -> \t" << timestamp << "\n";
	ss << "\tvalue -> \t" << value << "\n";
	return ss.str();
}

/////////////////////////////////////
///////////  TEMPERATURE  ///////////
/////////////////////////////////////
std::string Temperature::get_header(std::string separator)
{
	std::string header = "";
	header += "timestamp" + separator;
	for(int i = 0; i < 16; i++)
		header += "channel_" + std::to_string(i) + separator;
	return header;
}
std::string Temperature::get_string(std::string separator)
{
	std::string str = "";
	str += std::to_string(timestamp) + separator;
	for(int i = 0; i < 16; i++)
		str += std::to_string(temps[i]) + separator;
	return str;
}
Document Temperature::get_json()
{
	Document d;
	d.SetObject();
	Document::AllocatorType &alloc = d.GetAllocator();
	d.AddMember("timestamp", timestamp, alloc);
	Value v;
	v.SetArray();
	for(int i = 0; i < 16; i++)
		v.PushBack(temps[i], alloc);
	d.AddMember("value", v, alloc);
	return d;
}
void Temperature::serialize(devices::Temperature* temperature)
{
	temperature->set_timestamp(timestamp);

	temperature->clear_temps();
	for(int i = 0; i < 16; i++)
		temperature->add_temps(temps[i]);
}
std::string Temperature::get_readable()
{
	std::stringstream ss;
	ss.precision(3);
	ss << get_name() << "\n";
	ss << "\ttimestamp -> \t" << timestamp << "\n";
	for(int i = 0; i < 16; i++)
		ss << "\tchannel_" << i << " -> \t" << temps[i] << "\n";
	return ss.str();
}

///////////////////////////////
////////////  GPS  ////////////
///////////////////////////////
std::string Gps::get_header(std::string separator)
{
	std::stringstream ss;
	ss << "timestamp" + separator
	<< "msg_type" + separator
	<< "time" + separator
	<< "latitude" + separator
	<< "longitude" + separator
	<< "altitude" + separator
	<< "fix" + separator
	<< "satellites" + separator
	<< "fix_state" + separator
	<< "age_of_correction" + separator
	<< "course_over_ground_degrees" + separator
	<< "course_over_ground_degrees_magnetic" + separator
	<< "speed_kmh" + separator
	<< "mode" + separator
	<< "position_diluition_precision" + separator
	<< "horizontal_diluition_precision" + separator
	<< "vertical_diluition_precision" + separator;
	return ss.str();
}
std::string Gps::get_string(std::string separator)
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(9)
	<< timestamp << separator
	<< msg_type + separator
	<< time + separator
	<< latitude << separator
	<< longitude << separator
	<< altitude << separator
	<< fix << separator
	<< satellites << separator
	<< fix_state + separator
	<< age_of_correction << separator
	<< course_over_ground_degrees << separator
	<< course_over_ground_degrees_magnetic << separator
	<< speed_kmh << separator
	<< mode + separator
	<< position_diluition_precision << separator
	<< horizontal_diluition_precision << separator
	<< vertical_diluition_precision << separator;
	return ss.str();
}
Document Gps::get_json()
{
	Document d;
	d.SetObject();
	Document::AllocatorType &alloc = d.GetAllocator();
	d.AddMember("timestamp", timestamp, alloc);
	d.AddMember("msg_type", Value().SetString(msg_type.c_str(), msg_type.size(), alloc), alloc);
	d.AddMember("time", Value().SetString(time.c_str(), time.size(), alloc), alloc);
	d.AddMember("latitude", latitude, alloc);
	d.AddMember("longitude", longitude, alloc);
	d.AddMember("altitude", altitude, alloc);
	d.AddMember("fix", fix, alloc);
	d.AddMember("satellites", satellites, alloc);
	d.AddMember("fix_state", Value().SetString(fix_state.c_str(), fix_state.size(), alloc), alloc);
	d.AddMember("age_of_correction", age_of_correction, alloc);
	d.AddMember("course_over_ground_degrees", course_over_ground_degrees, alloc);
	d.AddMember("course_over_ground_degrees_magnetic", course_over_ground_degrees_magnetic, alloc);
	d.AddMember("speed_kmh", speed_kmh, alloc);
	d.AddMember("mode", Value().SetString(mode.c_str(), mode.size(), alloc), alloc);
	d.AddMember("position_diluition_precision", position_diluition_precision, alloc);
	d.AddMember("horizontal_diluition_precision", horizontal_diluition_precision, alloc);
	d.AddMember("vertical_diluition_precision", vertical_diluition_precision, alloc);

	return d;
}
void Gps::serialize(devices::Gps* gps)
{
	gps->set_timestamp(timestamp);
	gps->set_msg_type(msg_type);
	gps->set_time(time);
	gps->set_latitude(latitude);
	gps->set_longitude(longitude);
	gps->set_altitude(altitude);
	gps->set_fix(fix);
	gps->set_satellites(satellites);
	gps->set_fix_state(fix_state);
	gps->set_age_of_correction(age_of_correction);
	gps->set_course_over_ground_degrees(course_over_ground_degrees);
	gps->set_course_over_ground_degrees_magnetic(course_over_ground_degrees_magnetic);
	gps->set_speed_kmh(speed_kmh);
	gps->set_mode(mode);
	gps->set_position_diluition_precision(position_diluition_precision);
	gps->set_horizontal_diluition_precision(horizontal_diluition_precision);
	gps->set_vertical_diluition_precision(vertical_diluition_precision);
}
std::string Gps::get_readable()
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(3)
	<< get_name() << "\n"
	<< "\ttimestamp -> \t" << timestamp << "\n"
	<< "\tmsg_type -> \t" << msg_type << "\n"
	<< "\ttime -> \t" << time << "\n"
	<< "\tlatitude -> \t" << latitude << "\n"
	<< "\tlongitude -> \t" << longitude << "\n"
	<< "\taltitude -> \t" << altitude << "\n"
	<< "\tfix -> \t" << fix << "\n"
	<< "\tsatellites -> \t" << satellites << "\n"
	<< "\tfix_state -> \t" << fix_state << "\n"
	<< "\tage_of_correction -> \t" << age_of_correction << "\n"
	<< "\tcourse_over_ground_degrees -> \t" << course_over_ground_degrees << "\n"
	<< "\tcourse_over_ground_degrees_magnetic -> \t" << course_over_ground_degrees_magnetic << "\n"
	<< "\tspeed_kmh -> \t" << speed_kmh << "\n"
	<< "\tmode -> \t" << mode << "\n"
	<< "\tposition_diluition_precision -> \t" << position_diluition_precision << "\n"
	<< "\thorizontal_diluition_precision -> \t" << horizontal_diluition_precision << "\n"
	<< "\tvertical_diluition_precision -> \t" << vertical_diluition_precision << "\n";
	return ss.str();
}

