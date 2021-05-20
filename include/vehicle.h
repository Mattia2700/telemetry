#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>

#include "devices.h"

using namespace std;

class Chimera{
public:
  Chimera();

  Device* parse_message(double timestamp, int id, uint8_t data[], int size);

  void add_filenames(string base_path, string extension);
  void open_all_files();
  void close_all_files();
  void close_files(int index);

  void write_all_headers(int index);

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
