#include "devices.h"

int Device::global_id = 0;

Device::Device(string name)
{
  this->name = name;

  id = global_id;
  global_id++;
  count = 0;
}
Device::~Device() {}

string Imu::get_header(string separator)
{
  string ret = "";
  ret += "timestamp" + separator;
  ret += "x" + separator;
  ret += "y" + separator;
  ret += "z" + separator;
  ret += "scale";
  return ret;
}
string Imu::get_string(string separator)
{
  string ret = "";
  ret += to_string(timestamp) + separator;
  ret += to_string(x) + separator;
  ret += to_string(y) + separator;
  ret += to_string(z) + separator;
  ret += to_string(scale);
  return ret;
}
Document Imu::json()
{
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &alloc = d.GetAllocator();
  d.AddMember("timestamp", timestamp, alloc);
  d.AddMember("x", x, alloc);
  d.AddMember("y", y, alloc);
  d.AddMember("z", z, alloc);
  d.AddMember("scale", scale, alloc);
  return d;
}
void Imu::serialize(devices::Imu* imu){
  imu->set_timestamp(timestamp);
  imu->set_x(x);
  imu->set_y(y);
  imu->set_z(z);
  imu->set_scale(scale);
}

string Encoder::get_header(string separator)
{
  string ret = "";
  ret += "timestamp" + separator;
  ret += "rads" + separator;
  ret += "km" + separator;
  ret += "rotations";
  return ret;
}
string Encoder::get_string(string separator)
{
  string ret = "";
  ret += to_string(timestamp) + separator;
  ret += to_string(rads) + separator;
  ret += to_string(km) + separator;
  ret += to_string(rotations);
  return ret;
}
Document Encoder::json()
{
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &alloc = d.GetAllocator();
  d.AddMember("timestamp", timestamp, alloc);
  d.AddMember("rads", rads, alloc);
  d.AddMember("km", km, alloc);
  d.AddMember("rotations", rotations, alloc);
  return d;
}
void Encoder::serialize(devices::Encoder* encoder){
  encoder->set_timestamp(timestamp);
  encoder->set_rads(rads);
  encoder->set_km(km);
  encoder->set_rotations(rotations);
}

string Steer::get_header(string separator)
{
  string ret = "";
  ret += "timestamp" + separator;
  ret += "angle";
  return ret;
}
string Steer::get_string(string separator)
{
  string ret = "";
  ret += to_string(timestamp) + separator;
  ret += to_string(angle);
  return ret;
}
Document Steer::json()
{
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &alloc = d.GetAllocator();
  d.AddMember("timestamp", timestamp, alloc);
  d.AddMember("angle", angle, alloc);
  return d;
}
void Steer::serialize(devices::Steer* steer){
  steer->set_timestamp(timestamp);
  steer->set_angle(angle);
}

string Pedals::get_header(string separator)
{
  string ret = "";
  ret += "timestamp" + separator;
  ret += "throttle1" + separator;
  ret += "throttle2" + separator;
  ret += "front" + separator;
  ret += "back";
  return ret;
}
string Pedals::get_string(string separator)
{
  string ret = "";
  ret += to_string(timestamp) + separator;
  ret += to_string(throttle1) + separator;
  ret += to_string(throttle2) + separator;
  ret += to_string(brake_front) + separator;
  ret += to_string(brake_rear);
  return ret;
}
Document Pedals::json()
{
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &alloc = d.GetAllocator();
  d.AddMember("timestamp", timestamp, alloc);
  d.AddMember("throttle1", throttle1, alloc);
  d.AddMember("throttle2", throttle2, alloc);
  d.AddMember("brake_front", brake_front, alloc);
  d.AddMember("brake_rear", brake_rear, alloc);
  return d;
}
void Pedals::serialize(devices::Pedals *pedals){
  pedals->set_timestamp(timestamp);
  pedals->set_throttle1(throttle1);
  pedals->set_throttle2(throttle2);
  pedals->set_brake_front(brake_front);
  pedals->set_brake_rear(brake_rear);
}

string Ecu::get_header(string separator)
{
  string ret = "";
  return ret;
}
string Ecu::get_string(string separator)
{
  string ret = "";
  return ret;
}
Document Ecu::json()
{
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &alloc = d.GetAllocator();
  d.AddMember("timestamp", timestamp, alloc);
  return d;
}
void Ecu::serialize(devices::Ecu* ecu){

}

string Inverter::get_header(string separator)
{
  string ret = "";
  ret += "timestamp" + separator;
  ret += "temperature" + separator;
  ret += "motorTemp" + separator;
  ret += "torque" + separator;
  ret += "speed";
  return ret;
}
string Inverter::get_string(string separator)
{
  string ret = "";
  ret += to_string(timestamp) + separator;
  ret += to_string(temperature) + separator;
  ret += to_string(motor_temp) + separator;
  ret += to_string(torque) + separator;
  ret += to_string(speed);
  return ret;
}
Document Inverter::json()
{
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &alloc = d.GetAllocator();
  d.AddMember("timestamp", timestamp, alloc);
  d.AddMember("temperature", temperature, alloc);
  d.AddMember("motorTemp", motor_temp, alloc);
  d.AddMember("torque", torque, alloc);
  d.AddMember("speed", speed, alloc);
  return d;
}
void Inverter::serialize(devices::Inverter* inverter){
  inverter->set_timestamp(timestamp);
  inverter->set_motor_temp(motor_temp);
  inverter->set_torque(torque);
  inverter->set_speed(speed);
}

string Bms::get_header(string separator)
{
  string ret = "";
  ret += "timestamp" + separator;
  ret += "temperature" + separator;
  ret += "max temperature" + separator;
  ret += "voltage" + separator;
  ret += "current" + separator;
  ret += "power";
  return ret;
}
string Bms::get_string(string separator)
{
  string ret = "";
  ret += to_string(timestamp) + separator;
  ret += to_string(temperature) + separator;
  ret += to_string(max_temperature) + separator;
  ret += to_string(voltage) + separator;
  ret += to_string(current) + separator;
  ret += to_string(power);
  return ret;
}
Document Bms::json()
{
  Document d;
  d.SetObject();
  rapidjson::Document::AllocatorType &alloc = d.GetAllocator();
  d.AddMember("timestamp", timestamp, alloc);
  d.AddMember("temperature", temperature, alloc);
  d.AddMember("max_temperature", temperature, alloc);
  d.AddMember("voltage", voltage, alloc);
  d.AddMember("current", current, alloc);
  d.AddMember("power", power, alloc);
  return d;
}
void Bms::serialize(devices::Bms* bms){
  bms->set_timestamp(timestamp);
  bms->set_temperature(temperature);
  bms->set_max_temperature(max_temperature);
  bms->set_current(current);
  bms->set_voltage(voltage);
  bms->set_power(power);
}
