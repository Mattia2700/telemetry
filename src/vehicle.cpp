#include "vehicle.h"


Chimera::Chimera(){
  encoder = new Encoder();
  ecu = new Ecu();


  devices.push_back(encoder);
  devices.push_back(ecu);
}
