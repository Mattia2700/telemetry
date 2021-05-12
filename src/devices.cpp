#include "devices.h"

int Device::global_id = 0;

Device::Device(string name){
  this->name = name;

  id = global_id;
  global_id ++;
}
Device::~Device(){}

string Imu::get_string(string separator){
  string ret = "";
  ret += to_string(timestamp) + separator;
  ret += to_string(x) + separator;
  ret += to_string(y) + separator;
  ret += to_string(z) + separator;
  ret += to_string(scale);
  return ret;
}

string Encoder::get_string(string separator){
  string ret = "";
  ret += to_string(timestamp) + separator;
  ret += to_string(rads) + separator;
  ret += to_string(km) + separator;
  ret += to_string(rotations);
  return ret;
}
string Steer::get_string(string separator){
  string ret = "";
  ret += to_string(timestamp) + separator;
  ret += to_string(angle);
  return ret;
}
string Pedals::get_string(string separator){
  string ret = "";
  ret += to_string(timestamp) + separator;
  ret += to_string(throttle1) + separator;
  ret += to_string(throttle2) + separator;
  ret += to_string(brake_front) + separator;
  ret += to_string(brake_rear);
  return ret;
}
string Ecu::get_string(string separator){
  string ret = "";
  return ret;
}
string Inverter::get_string(string separator){
  string ret = "";
  ret += to_string(timestamp) + separator;
  ret += to_string(temperature) + separator;
  ret += to_string(motorTemp) + separator;
  ret += to_string(torque) + separator;
  ret += to_string(speed);
  return ret;
}
string Bms::get_string(string separator){
  string ret = "";
  ret += to_string(timestamp) + separator;
  ret += to_string(temperature) + separator;
  ret += to_string(current) + separator;
  ret += to_string(voltage) + separator;
  ret += to_string(power);
  return ret;
}
