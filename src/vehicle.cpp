#include "vehicle.h"


Chimera::Chimera(){

  // Device initialization
  gyro  = new Imu("Gyro");
  accel = new Imu("Accel");
  bms_lv = new Bms("BMS LV");
  bms_hv = new Bms("BMS HV");
  steer = new Steer("Steer");
  pedal = new Pedals("Pedals");
  encoder_left  = new Encoder("Encoder Left");
  encoder_right = new Encoder("Encoder Right");
  inverter_left  = new Inverter("Inverter Left");
  inverter_right = new Inverter("Inverter Right");
  ecu_state = new State("State ECU");
  bms_hv_state = new State("State BMS HV");
  steering_wheel_state = new State("State Steering Wheel");
  ecu = new Ecu("ECU");


  // Device list
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
  devices.push_back(ecu_state);
  devices.push_back(bms_hv_state);
  devices.push_back(steering_wheel_state);
  devices.push_back(ecu);

  // Protobuffer section

  // Initializing chimera protobuffer
  chimera_proto = new devices::Chimera();

  // Initialize vector containing protobuffer (message) of each device
  // Must be in the same order as the vector of devices
  proto_messages.push_back(new devices::Imu());
  proto_messages.push_back(new devices::Imu());
  proto_messages.push_back(new devices::Encoder());
  proto_messages.push_back(new devices::Encoder());
  proto_messages.push_back(new devices::Inverter());
  proto_messages.push_back(new devices::Inverter());
  proto_messages.push_back(new devices::Bms());
  proto_messages.push_back(new devices::Bms());
  proto_messages.push_back(new devices::Pedals());
  proto_messages.push_back(new devices::Steer());
  proto_messages.push_back(new devices::State());
  proto_messages.push_back(new devices::State());
  proto_messages.push_back(new devices::State());
  proto_messages.push_back(new devices::Ecu());

  if(devices.size() != proto_messages.size()){
    throw logic_error("Chimera initialization incorrect, vectors of devices and messages not of the same size");
  }

  // Get all names of the chimera message
  auto descriptor = chimera_proto->GetDescriptor();
  string separator = ";";
  for(int i = 0; i < descriptor->field_count(); i++){
    string field = descriptor->field(i)->json_name();
    auto descriptor = proto_messages[i]->GetDescriptor();

    proto_names.push_back(field);
    device_headers.insert({field, Device::get_header(descriptor, separator)});
  }
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

vector<Device *> Chimera::parse_message(double& timestamp, const int &id, uint8_t data[], const int &size){
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

      gyro->x /= 100.0f;
      gyro->y /= 100.0f;
      gyro->z /= 100.0f;

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

      accel->x /= 100.0f;
      accel->y /= 100.0f;
      accel->z /= 100.0f;

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

        pedal->brake_front /= 500.0f;
        pedal->brake_rear /= 500.0f;

        modifiedDevices.push_back(pedal);
      }
    break;
    case 0xC0:
      if(data[0] == 0x02){
        steer->angle = (data[1] << 8) + data[2];

        steer->angle /= 100.0f;

        steer->timestamp = timestamp;

        modifiedDevices.push_back(steer);
      }
    break;
    case 0xD0:
      encoder_left->rads =  (data[0] << 16) + (data[1] << 8) + data[2];
      encoder_right->rads = (data[3] << 16) + (data[4] << 8) + data[5];

      encoder_left->rads /= 10000.0f;
      encoder_right->rads /= 10000.0f;
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
        bms_hv->voltage     = ((data[1] << 16) + (data[2] << 8) + data[3])/10000.0f;
        bms_hv->max_voltage = ((data[4] << 8)  +  data[5]) / 10000.0f;
        bms_hv->min_voltage = ((data[6] << 8)  +  data[7]) / 10000.0f;
        bms_hv->timestamp   =   timestamp;
        modifiedDevices.push_back(bms_hv);
      }else if(data[0] == 0x03){
        bms_hv_state->value = "TS ON";
        bms_hv_state->timestamp = timestamp;
        modifiedDevices.push_back(bms_hv_state);
      }else if(data[0] == 0x04){
        bms_hv_state->value = "TS OFF";
        bms_hv_state->timestamp = timestamp;
        modifiedDevices.push_back(bms_hv_state);
      }else if(data[0] == 0x09){
        bms_hv_state->value = BMS_WARNINGS[data[1]] + " -> " + to_string(data[2]);
        bms_hv_state->timestamp = timestamp;
        modifiedDevices.push_back(bms_hv_state);
      }else if(data[0] == 0x08){
        bms_hv_state->value = BMS_ERRORS[data[1]] + " -> " + to_string(data[2]);
        bms_hv_state->timestamp = timestamp;
        modifiedDevices.push_back(bms_hv_state);
      }else if(data[0] == 0x05){
        bms_hv->current = ((data[1] << 8) + data[2])/10.0f;
        bms_hv->power   =  (data[3] << 8) + data[4];
        bms_hv->timestamp = timestamp;
        modifiedDevices.push_back(bms_hv);
      }else if(data[0] == 0x0A){
        bms_hv->temperature     = ((data[1] << 8) + (data[2])) / 100.0f;
        bms_hv->max_temperature = ((data[3] << 8) + (data[4])) / 100.0f;
        bms_hv->min_temperature = ((data[5] << 8) + (data[6])) / 100.0f;
        bms_hv->timestamp       =   timestamp;
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
    case 0xA0:
      if(data[0] == 0x03){
        steering_wheel_state->value = "REQUEST TS ON";
        steering_wheel_state->timestamp = timestamp;
        modifiedDevices.push_back(steering_wheel_state);
      }else if(data[0] == 0x04){
        steering_wheel_state->value = "REQUEST TO IDLE";
        steering_wheel_state->timestamp = timestamp;
        modifiedDevices.push_back(steering_wheel_state);
      }else if(data[0] == 0x05){
        steering_wheel_state->value = "REQUEST TO RUN";
        steering_wheel_state->timestamp = timestamp;
        modifiedDevices.push_back(steering_wheel_state);
      }else if(data[0] == 0x06){
        steering_wheel_state->value = "REQUEST TO SETUP";
        steering_wheel_state->timestamp = timestamp;
        modifiedDevices.push_back(steering_wheel_state);
      }else if(data[0] == 0x08){
        steering_wheel_state->value = "REQUEST INVERTER LEFT ON";
        steering_wheel_state->timestamp = timestamp;
        modifiedDevices.push_back(steering_wheel_state);
      }else if(data[0] == 0x09){
        steering_wheel_state->value = "REQUEST INVERTER RIGHT ON";
        steering_wheel_state->timestamp = timestamp;
        modifiedDevices.push_back(steering_wheel_state);
      }
    break;
    case 0x55:
      if(data[0] == 0x01){
        ecu_state->value = "STATE: " + ECU_STATES[data[4]] + " Map: " + to_string(data[3]);
        ecu_state->timestamp = timestamp;
        modifiedDevices.push_back(ecu_state);
      }else if(data[0] == 0x0B && data[1] == 0x04){
        ecu_state->value = "REQUEST TS OFF";
        ecu_state->timestamp = timestamp;
        modifiedDevices.push_back(ecu_state);
      }else if(data[0] == 0x0B && data[1] == 0x08){
        ecu_state->value = "REQUEST TS OFF ERRORS";
        ecu_state->timestamp = timestamp;
        modifiedDevices.push_back(ecu_state);
      }else if(data[0] == 0x0A){
        ecu_state->value = "REQUEST TS ON";
        ecu_state->timestamp = timestamp;
        modifiedDevices.push_back(ecu_state);
      }
    break;
    case 0x201:
      if(data[0] == 0x90){
        ecu->power_request_left = (data[2] << 8) + data[1];
        ecu->timestamp = timestamp;
        modifiedDevices.push_back(ecu);
      }
    break;
    case 0x202:
      if(data[0] == 0x90){
        ecu->power_request_right = (data[2] << 8) + data[1];
        ecu->timestamp = timestamp;
        modifiedDevices.push_back(ecu);
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

  this->ecu_state->serialize(chimera_proto->add_ecu_state());
  this->bms_hv_state->serialize(chimera_proto->add_bms_hv_state());
  this->steering_wheel_state->serialize(chimera_proto->add_steering_wheel_state());

  this->ecu->serialize(chimera_proto->add_ecu());
}


void Chimera::serialize_device(Device* device){
  if (device == accel){
    this->accel->serialize(chimera_proto->add_accel());
  }else if(device == gyro){
    this->gyro->serialize(chimera_proto->add_gyro());
  }else if(device == encoder_left){
    this->encoder_left->serialize(chimera_proto->add_encoder_left());
  }else if(device == encoder_right){
    this->encoder_right->serialize(chimera_proto->add_encoder_right());
  }else if(device == inverter_left){
    this->inverter_left->serialize(chimera_proto->add_inverter_left());
  }else if(device == inverter_right){
    this->inverter_right->serialize(chimera_proto->add_inverter_right());
  }else if(device == bms_lv){
    this->bms_lv->serialize(chimera_proto->add_bms_lv());
  }else if(device == bms_hv){
    this->bms_hv->serialize(chimera_proto->add_bms_hv());
  }else if(device == pedal){
    this->pedal->serialize(chimera_proto->add_pedal());
  }else if(device == steer){
    this->steer->serialize(chimera_proto->add_steer());
  }else if(device == ecu_state){
    this->ecu_state->serialize(chimera_proto->add_ecu_state());
  }else if(device == bms_hv_state){
    this->bms_hv_state->serialize(chimera_proto->add_bms_hv_state());
  }else if(device == steering_wheel_state){
    this->steering_wheel_state->serialize(chimera_proto->add_steering_wheel_state());
  }else if(device == ecu){
    this->ecu->serialize(chimera_proto->add_ecu());
  }
}
