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
    case 0x4EC:
      gyro->scale = 8;
      gyro->x = (data[0] << 8) + data[1];
      gyro->y = (data[2] << 8) + data[3];
      gyro->z = (data[4] << 8) + data[5];

      if(gyro->x > 32768)
        gyro->x -= 65536;
      if(gyro->y > 32768)
        gyro->y -= 65536;
      if(gyro->z > 32768)
        gyro->z -= 65536;

      gyro->x /= 100;
      gyro->y /= 100;
      gyro->z /= 100;

      gyro->timestamp = timestamp;
    break;
    case 0x4ED:
      accel->scale = 245;
      accel->x = (data[0] << 8) + data[1];
      accel->y = (data[2] << 8) + data[3];
      accel->z = (data[4] << 8) + data[5];

      if(accel->x > 32768)
        accel->x -= 65536;
      if(accel->y > 32768)
        accel->y -= 65536;
      if(accel->z > 32768)
        accel->z -= 65536;

      accel->x /= 100;
      accel->y /= 100;
      accel->z /= 100;

      accel->timestamp = timestamp;
    break;
    case 0xB0: // Pedals
      if(data[0] == 0x01){
        pedal->timestamp = timestamp;

        pedal->throttle1 = data[1];
        pedal->throttle2 = data[2];
        // count ++;
      }
      else if(data[0] == 0x02){
        pedal->timestamp = timestamp;

        pedal->brake_front = (data[2] << 8) + data[4];
        pedal->brake_rear  = (data[5] << 8) + data[7];

        pedal->brake_front /= 500;
        pedal->brake_rear /= 500;
      }
    break;
    case 0xC0:
      if(data[0] == 0x02){
        steer->angle = (data[1] << 8) + data[2];

        steer->angle /= 100;

        steer->timestamp = timestamp;
      }
    break;
    case 0xD0:
      if(data[0] == 0x07){
        encoder_left->rads =  (data[1] << 16) + (data[2] << 8) + data[3];
        encoder_right->rads = (data[4] << 16) + (data[5] << 8) + data[6];

        encoder_left->rads /= 10000;
        encoder_right->rads /= 10000;
        if(data[7] == 1){
          encoder_left->rads *= -1;
          encoder_right->rads *= -1;
        }

        encoder_left->timestamp = timestamp;
        encoder_right->timestamp = timestamp;
      }
      else if(data[0] == 0x08){
        encoder_right->km = (data[1] << 8) + data[2];
        encoder_right->rotations = (data[3] << 8) + data[4];

        encoder_left->km = encoder_right->km;
        encoder_left->rotations = encoder_right->rotations;
      }
    break;
    case 0xAA:
      if(data[0] == 0x01){
        bms_hv->voltage = ((data[1] << 16) + (data[2] << 8))/10000;
        bms_hv->timestamp = timestamp;
      }else if(data[0] == 0x05){
        bms_hv->current = ((data[1] << 16) + (data[2]))/10;
        bms_hv->power = data[3] << 8 + data[4];
        bms_hv->timestamp = timestamp;
      }else if(data[0] == 0xA0){
        bms_hv->temperature = ((data[1] << 8) + (data[2]))/10;
        bms_hv->timestamp = timestamp;
      }
    break;
    case 0xFF:
      bms_lv->voltage = data[0]/10;
      bms_lv->temperature = data[1]/5;
      bms_lv->timestamp = timestamp;
    break;
  }
}
