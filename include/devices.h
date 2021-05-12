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

using namespace std;
using namespace std::chrono;

class Device {
public:
  Device();
  virtual ~Device();

  virtual string name()=0;

  int get_id(){return id;};

private:
  int id;
  static int global_id;
};

class Encoder: public Device {
public:
  Encoder(): Device(){};
  ~Encoder();

  virtual string name(){return "Encoder";};

};

class Ecu: public Device {
public:
  Ecu(): Device(){};
  ~Ecu();

  virtual string name(){return "Ecu";};
};



#endif //DEVICES_H
