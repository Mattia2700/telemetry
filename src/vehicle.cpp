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


void Chimera::set_all_filenames(string base_path, string extension){
  for(auto device : devices){
    device->filename = base_path + "/" + device->get_name() + extension;
  }
}

void Chimera::open_all_files(){
  for(auto device : devices){
    device->file = new fstream(device->filename, fstream::out);
  }
}

void Chimera::close_all_files(){
  for(auto device : devices){
    device->file->close();
  }
}

Device* Chimera::parse_message(double timestamp, int id, uint8_t data[], int size){
  switch (id) {
    case 0x4EC:
      gyro->scale = 245;
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
      return gyro;
    break;
    case 0x4ED:
      accel->scale = 8;
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

      return accel;
    break;
    case 0xB0: // Pedals
      if(data[0] == 0x01){
        pedal->throttle1 = data[1];
        pedal->throttle2 = data[2];

        pedal->timestamp = timestamp;

        return pedal;
      }
      else if(data[0] == 0x02){
        pedal->timestamp = timestamp;

        pedal->brake_front = (data[2] << 8) + data[4];
        pedal->brake_rear  = (data[5] << 8) + data[7];

        pedal->brake_front /= 500;
        pedal->brake_rear /= 500;

        return pedal;
      }
    break;
    case 0xC0:
      if(data[0] == 0x02){
        steer->angle = (data[1] << 8) + data[2];

        steer->angle /= 100;

        steer->timestamp = timestamp;

        return steer;
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

        return encoder_right;
      }
      else if(data[0] == 0x08){
        encoder_right->km = (data[1] << 8) + data[2];
        encoder_right->rotations = (data[3] << 8) + data[4];

        encoder_left->km = encoder_right->km;
        encoder_left->rotations = encoder_right->rotations;

        return encoder_left;
      }
    break;
    case 0xAA:
      if(data[0] == 0x01){
        bms_hv->voltage = ((data[1] << 16) + (data[2] << 8))/10000;
        bms_hv->timestamp = timestamp;
        return bms_hv;
      }else if(data[0] == 0x05){
        bms_hv->current = ((data[1] << 16) + (data[2]))/10;
        bms_hv->power = data[3] << 8 + data[4];
        bms_hv->timestamp = timestamp;
        return bms_hv;
      }else if(data[0] == 0xA0){
        bms_hv->temperature = ((data[1] << 8) + (data[2]))/10;
        bms_hv->timestamp = timestamp;
        return bms_hv;
      }
    break;
    case 0xFF:
      bms_lv->voltage = data[0]/10;
      bms_lv->temperature = data[1]/5;
      bms_lv->timestamp = timestamp;
      return bms_lv;
    break;
    case 0x181:
      if(data[0] == 0xA0){
        inverter_left->torque = (data[2] << 8) + data[1];
        if(inverter_left->torque > 32767)
          inverter_left->torque -= 65535;
        inverter_left->timestamp = timestamp;
        return inverter_left;
      }
      else if(data[0] == 0x4A){
        inverter_left->temperature = ((data[2] << 8) + data[1] - 15797) / 112.1182;
        inverter_left->timestamp = timestamp;
        return inverter_left;
      }
      else if(data[0] == 0x49){
        inverter_left->motorTemp = ((data[2] << 8) + data[1] - 9393.9)/55.1;
        inverter_left->timestamp = timestamp;
        return inverter_left;
      }
      else if(data[0] == 0xA8){
        inverter_left->speed = (data[2] << 8) + data[1];
        if(inverter_left->speed > 32768)
          inverter_left->speed -= 65535;
        inverter_left->timestamp = timestamp;
        return inverter_left;
      }
    break;
    case 0x182:
      if(data[0] == 0xA0){
        inverter_right->torque = (data[2] << 8) + data[1];
        if(inverter_right->torque > 32767)
          inverter_right->torque -= 65535;
        inverter_right->timestamp = timestamp;
        return inverter_right;
      }
      else if(data[0] == 0x4A){
        inverter_right->temperature = ((data[2] << 8) + data[1] - 15797) / 112.1182;
        inverter_right->timestamp = timestamp;
        return inverter_right;
      }
      else if(data[0] == 0x49){
        inverter_right->motorTemp = ((data[2] << 8) + data[1] - 9393.9)/55.1;
        inverter_right->timestamp = timestamp;
        return inverter_right;
      }
      else if(data[0] == 0xA8){
        inverter_right->speed = (data[2] << 8) + data[1];
        if(inverter_right->speed > 32768)
          inverter_right->speed -= 65535;
        inverter_right->timestamp = timestamp;
        return inverter_right;
      }
    break;
    default:
    return nullptr;
    break;
  }
  return nullptr;
}
