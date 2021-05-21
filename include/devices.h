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

using namespace std;
using namespace std::chrono;

class Device {
public:
  Device(string name="default");
  virtual ~Device();

  //virtual string name()=0;

  int get_id(){return id;};
  string get_name(){return name;};

  virtual string get_header(string separator)=0;
  virtual string get_string(string separator)=0;

  double timestamp;

  double count;
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

  //virtual string name(){return "IMU";};
  virtual string get_header(string separator);
  virtual string get_string(string separator);

  double x;
  double y;
  double z;
  double scale;
};

class Encoder: public Device {
public:
  Encoder(string name="default"): Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);

  double rads;
  double km;
  double rotations;
};

class Steer: public Device {
public:
  Steer(string name="default"): Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);

  double angle;
};

class Pedals: public Device {
public:
  Pedals(string name="default"): Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);

  double throttle1;
  double throttle2;
  double brake_front;
  double brake_rear;
};

class Ecu: public Device {
public:
  Ecu(string name="default"): Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);
};

class Inverter: public Device {
public:
  Inverter(string name="default"): Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);

  double temperature;
  double motorTemp;
  double torque;
  double speed;
};

class Bms: public Device {
public:
  Bms(string name="default"): Device(name){};

  virtual string get_header(string separator);
  virtual string get_string(string separator);

  double temperature;
  double current;
  double voltage;
  double power;
};

#endif //DEVICES_H
