#include "devices.h"

int Device::global_id = 0;

Device::Device(string name){
  this->name = name;

  id = global_id;
  global_id ++;
}
Device::~Device(){}
