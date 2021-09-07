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

using namespace std;
using namespace std::chrono;

class Device {
public:
  Device(string name="default");
  virtual ~Device();

  int get_id(){return id;};
  string get_name(){return name;};

  virtual string get_header(string separator)=0;
  virtual string get_string(string separator)=0;
  virtual Document json()=0;
  virtual string json_string(){
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    json().Accept(writer);

    return buffer.GetString();
  }

  float timestamp;

  float count;
  float helper_variable = 0;
  vector<std::fstream*> files;
  vector<string> filenames;
  vector<string> mesages_list;

private:
  string name;

  int id;
  static int global_id;

};

class Imu: public Device{
public:
  Imu(string name="default"): Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
  virtual Document json();

  void serialize(devices::Imu*);

  float x;
  float y;
  float z;
  float scale;

};

class Encoder: public Device {
public:
  Encoder(string name="default"): Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
  virtual Document json();

  void serialize(devices::Encoder*);

  float rads;
  float km;
  float rotations;

};

class Steer: public Device {
public:
  Steer(string name="default"): Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
  virtual Document json();

  void serialize(devices::Steer*);

  float angle;
};

class Pedals: public Device {
public:
  Pedals(string name="default"): Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
  virtual Document json();

  void serialize(devices::Pedals*);

  float throttle1;
  float throttle2;
  float brake_front;
  float brake_rear;
};

class Ecu: public Device {
public:
  Ecu(string name="default"): Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
  virtual Document json();

  void serialize(devices::Ecu*);
};

class Inverter: public Device {
public:
  Inverter(string name="default"): Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
  virtual Document json();

  void serialize(devices::Inverter*);

  float temperature;
  float motor_temp;
  float torque;
  float speed;
};

class Bms: public Device {
public:
  Bms(string name="default"): Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
  virtual Document json();

  void serialize(devices::Bms*);

  float temperature;
  float max_temperature;
  float current;
  float voltage;
  float power;
};

#endif //DEVICES_H
