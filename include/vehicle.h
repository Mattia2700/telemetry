#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>

#include "devices.h"

using namespace std;

class Chimera{
public:
  Chimera();

  /**
  * Fills the sensor values basing on ids and payload
  *
  * @param timestamp value that will be setted to the sensor (can be current timestamp or the one in the logfile)
  * @param id integer defining CAN message id
  * @param data is the message payload
  * @param size of the payload
  * return vector containing pointers to devices modified with this message
  */
  vector<Device*> parse_message(double timestamp, int id, uint8_t data[], int size);

  /**
  * Defines a filename to every device called DeviceName.extension
  * Used when creating csv
  *
  * @param base_path folder in which the file will be defined
  * @param extension of the filename
  */
  void add_filenames(string base_path, string extension);
  /**
  * Opens all the files of each device
  */
  void open_all_files();
  /**
  * Closes all files of each device
  */
  void close_all_files();
  /**
  * Closes the file index of every device.
  * So if added 2 filenames index can be 0 or 1

  * @param index of the files of each device
  */
  void close_files(int index);

  /**
  * Writes CSV header to the index of the file
  *
  * @param index of the files of each device
  */
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

  /**
  * Vector containing all the devices
  */
  vector<Device*> devices;
};

class Fenice{

};



#endif //VEHICLE_H
