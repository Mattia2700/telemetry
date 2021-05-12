#include "devices.h"

int Device::global_id = 0;

Device::Device(){
  id = global_id;
  global_id ++;
}
Device::~Device(){}


Encoder::~Encoder(){}
Ecu::~Ecu(){}
