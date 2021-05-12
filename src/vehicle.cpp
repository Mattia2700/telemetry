#include "vehicle.h"


Chimera::Chimera(){
  gyro  = new Imu("Gyro");
  accel = new Imu("Accel");

  encoder_left  = new Encoder("Encoder Left");
  encoder_right = new Encoder("Encoder Right");

  inverter_left  = new Inverter("Inverter Left");
  inverter_right = new Inverter("Inverter Right");

  bms_lv = new Bms("BMS LV");
  bms_hv = new Bms("BMS HV");

  pedal = new Pedals("Pedals");
  steer = new Steer("Steer");

  ecu = new Ecu("Ecu");

  devices.push_back(gyro);
  devices.push_back(accel);
  devices.push_back(encoder_left);
  devices.push_back(encoder_right);
  devices.push_back(inverter_left);
  devices.push_back(inverter_right);
  devices.push_back(bms_lv);
  devices.push_back(bms_hv);
  devices.push_back(pedal);
  devices.push_back(steer);
  devices.push_back(ecu);
}


void Chimera::parse_message(double timestamp, int id, uint8_t data[], int size){
  switch (id) {
    case 0xB0: // Pedals
      if(data[0] == 0x01){
        pedal->timestamp = timestamp;

        pedal->throttle1 = data[1];
        pedal->throttle2 = data[2];
        // count ++;
      }
      else if(data[0] == 0x02){
        pedal->timestamp = timestamp;

        pedal->brake_front = data[2] << 8 + data[4];
        pedal->brake_rear  = data[5] << 8 + data[7];

        pedal->brake_front /= 500;
        pedal->brake_rear /= 500;
      }
    break;
    case 0xD0:
    break;
  }
}
