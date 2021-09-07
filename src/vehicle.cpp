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

  chimera_proto = new devices::Chimera();
}


void Chimera::add_filenames(string base_path, string extension){
  for(auto device : devices){
    device->filenames.push_back(base_path + "/" + device->get_name() + extension);
  }
}

void Chimera::open_all_files(){
  for(auto device : devices)
    for(auto filename : device->filenames)
      device->files.push_back(new fstream(filename, fstream::out));
}

void Chimera::close_all_files(){
  for(auto device : devices){
    for(auto file : device->files){
      file->close();
      delete file;
    }

    device->files.clear();
    device->filenames.clear();
  }
}

void Chimera::close_files(int index){
  for(auto device : devices){
    device->files[index]->close();
    device->files.erase(device->files.begin() + index);
    device->filenames.erase(device->filenames.begin() + index);
  }
}

void Chimera::write_all_headers(int index){
  for(auto device : devices)
    *device->files[index] << device->get_header(";") << "\n";
}

vector<Device *> Chimera::parse_message(const float &timestamp, const int &id, uint8_t data[], const int &size){
  modifiedDevices.clear();

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

      modifiedDevices.push_back(gyro);
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

      modifiedDevices.push_back(accel);
    break;
    case 0xB0: // Pedals
      if(data[0] == 0x01){
        pedal->throttle1 = data[1];
        pedal->throttle2 = data[2];

        pedal->timestamp = timestamp;

        modifiedDevices.push_back(pedal);
      }
      else if(data[0] == 0x02){
        pedal->timestamp = timestamp;

        pedal->brake_front = (data[2] << 8) + data[4];
        pedal->brake_rear  = (data[5] << 8) + data[7];

        pedal->brake_front /= 500;
        pedal->brake_rear /= 500;

        modifiedDevices.push_back(pedal);
      }
    break;
    case 0xC0:
      if(data[0] == 0x02){
        steer->angle = (data[1] << 8) + data[2];

        steer->angle /= 100;

        steer->timestamp = timestamp;

        modifiedDevices.push_back(steer);
      }
    break;
    case 0xD0:
      encoder_left->rads =  (data[0] << 16) + (data[1] << 8) + data[2];
      encoder_right->rads = (data[3] << 16) + (data[4] << 8) + data[5];

      encoder_left->rads /= 10000;
      encoder_right->rads /= 10000;
      if(data[6] == 1)
        encoder_left->rads *= -1;
      if(data[7] == 1)
        encoder_right->rads *= -1;

      encoder_left->timestamp = timestamp;
      encoder_right->timestamp = timestamp;

      modifiedDevices.push_back(encoder_left);
      modifiedDevices.push_back(encoder_right);
      break;
    case 0xD1:
      encoder_left->rotations =  (data[0] << 16) + (data[1] << 8) + data[2];
      encoder_left->km = (data[3] << 16) + (data[4] << 8) + data[5];
      encoder_right->rotations =  (data[0] << 16) + (data[1] << 8) + data[2];
      encoder_right->km = (data[3] << 16) + (data[4] << 8) + data[5];
      // error left data[6]
      // error right data[6]

      modifiedDevices.push_back(encoder_left);
      modifiedDevices.push_back(encoder_right);
      break;
    case 0xAA:
      if(data[0] == 0x01){
        bms_hv->voltage = ((data[1] << 16) + (data[2] << 8))/10000;
        bms_hv->timestamp = timestamp;
        modifiedDevices.push_back(bms_hv);
      }else if(data[0] == 0x05){
        bms_hv->current = ((data[1] << 16) + (data[2]))/10;
        bms_hv->power = data[3] << 8 + data[4];
        bms_hv->timestamp = timestamp;
        modifiedDevices.push_back(bms_hv);
      }else if(data[0] == 0xA0){
        bms_hv->temperature = ((data[1] << 8) + (data[2]))/10;
        bms_hv->timestamp = timestamp;
        modifiedDevices.push_back(bms_hv);
      }
    break;
    case 0xFF:
      bms_lv->voltage = data[0]/10.0f;
      bms_lv->temperature = data[2]/5.0f;
      bms_lv->max_temperature = data[3]/5.0f;
      bms_lv->timestamp = timestamp;
      modifiedDevices.push_back(bms_lv);
    break;
    case 0x181:
      if(data[0] == 0xA0){
        inverter_left->torque = (data[2] << 8) + data[1];
        if(inverter_left->torque > 32767)
          inverter_left->torque -= 65535;
        inverter_left->timestamp = timestamp;
        modifiedDevices.push_back(inverter_left);
      }
      else if(data[0] == 0x4A){
        inverter_left->temperature = ((data[2] << 8) + data[1] - 15797) / 112.1182;
        inverter_left->timestamp = timestamp;
        modifiedDevices.push_back(inverter_left);
      }
      else if(data[0] == 0x49){
        inverter_left->motor_temp = ((data[2] << 8) + data[1] - 9393.9)/55.1;
        inverter_left->timestamp = timestamp;
        modifiedDevices.push_back(inverter_left);
      }
      else if(data[0] == 0xA8){
        inverter_left->speed = (data[2] << 8) + data[1];
        if(inverter_left->speed > 32768)
          inverter_left->speed -= 65535;
        inverter_left->timestamp = timestamp;
        modifiedDevices.push_back(inverter_left);
      }
    break;
    case 0x182:
      if(data[0] == 0xA0){
        inverter_right->torque = (data[2] << 8) + data[1];
        if(inverter_right->torque > 32767)
          inverter_right->torque -= 65535;
        inverter_right->timestamp = timestamp;
        modifiedDevices.push_back(inverter_right);
      }
      else if(data[0] == 0x4A){
        inverter_right->temperature = ((data[2] << 8) + data[1] - 15797) / 112.1182;
        inverter_right->timestamp = timestamp;
        modifiedDevices.push_back(inverter_right);
      }
      else if(data[0] == 0x49){
        inverter_right->motor_temp = ((data[2] << 8) + data[1] - 9393.9)/55.1;
        inverter_right->timestamp = timestamp;
        modifiedDevices.push_back(inverter_right);
      }
      else if(data[0] == 0xA8){
        inverter_right->speed = (data[2] << 8) + data[1];
        if(inverter_right->speed > 32768)
          inverter_right->speed -= 65535;
        inverter_right->timestamp = timestamp;
        modifiedDevices.push_back(inverter_right);
      }
    break;
    default:
    break;
  }
  return modifiedDevices;
}

void Chimera::serialize(){
  this->accel->serialize(chimera_proto->add_accel());
  this->gyro->serialize(chimera_proto->add_gyro());

  this->encoder_left->serialize(chimera_proto->add_encoder_left());
  this->encoder_right->serialize(chimera_proto->add_encoder_right());

  this->bms_lv->serialize(chimera_proto->add_bms_lv());
  this->bms_hv->serialize(chimera_proto->add_bms_hv());

  this->inverter_left->serialize(chimera_proto->add_inverter_left());
  this->inverter_right->serialize(chimera_proto->add_inverter_right());

  this->pedal->serialize(chimera_proto->add_pedal());
  this->steer->serialize(chimera_proto->add_steer());

  this->ecu->serialize(chimera_proto->add_ecu());
}
