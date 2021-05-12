#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>

#include "devices.h"

using namespace std;

class Chimera{
public:
  Chimera();
  Encoder * encoder;
  Ecu* ecu;

  vector<Device*> devices;
};

class Fenice{

};



#endif //VEHICLE_H
