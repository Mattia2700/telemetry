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

  string get_name(){return name;};

  int get_id(){return id;};

  double count;
  string filename;
  double timestamp;
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

  double x;
  double y;
  double z;
  double scale;
};

class Encoder: public Device {
public:
  Encoder(string name="default"): Device(name){};

  double rads;
  double km;
  double rotations;
};

class Steer: public Device {
public:
  Steer(string name="default"): Device(name){};

  double angle;
};

class Pedals: public Device {
public:
  Pedals(string name="default"): Device(name){};

  double throttle1;
  double throttle2;
  double brake_front;
  double brake_rear;
};

class Ecu: public Device {
public:
  Ecu(string name="default"): Device(name){};
};

class Inverter: public Device {
public:
  Inverter(string name="default"): Device(name){};

  double temperature;
  double motorTemp;
  double torque;
  double speed;
};

class Bms: public Device {
public:
  Bms(string name="default"): Device(name){};

  double temperature;
  double current;
  double voltage;
  double power;
};

#endif //DEVICES_H
