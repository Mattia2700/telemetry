#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>

#include "devices.h"

using namespace std;

class Chimera{
public:
  Chimera();

  void parse_message(double timestamp, int id, uint8_t data[], int size);

  Imu* accel;
  Imu* gyro;

  Encoder* encoder_left;
  Encoder* encoder_right;

  Bms* bms_lv;
  Bms* bms_hv;

  Inverter* inverter_left;
  Inverter* inverter_right;

  Pedals* pedal;
  Steer* steer;

  Ecu* ecu;

  vector<Device*> devices;
};

class Fenice{

};



#endif //VEHICLE_H
