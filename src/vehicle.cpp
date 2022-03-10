#include "vehicle.h"


Chimera::Chimera(){

  // Device initialization
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
  ecu_state = new State("State ECU");
  bms_hv_state = new State("State BMS HV");
  steering_wheel_state = new State("State Steering Wheel");
  ecu = new Ecu("ECU");
  temp_fl = new Temperature("Temperature FL");
  temp_fr = new Temperature("Temperature FR");
  temp_rl = new Temperature("Temperature RL");
  temp_rr = new Temperature("Temperature RR");
  gps1 = new Gps("GPS1");
  gps2 = new Gps("GPS2");


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
  devices.push_back(temp_fl);
  devices.push_back(temp_fr);
  devices.push_back(temp_rl);
  devices.push_back(temp_rr);
  devices.push_back(gps1);
  devices.push_back(gps2);

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
  proto_messages.push_back(new devices::Temperature());
  proto_messages.push_back(new devices::Temperature());
  proto_messages.push_back(new devices::Temperature());
  proto_messages.push_back(new devices::Temperature());
  proto_messages.push_back(new devices::Gps());
  proto_messages.push_back(new devices::Gps());

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
      device->files.push_back(new std::fstream(filename, std::fstream::out));
}

void Chimera::close_all_files(){
  for(Device* device : devices){
    for(auto file : device->files){
      if(file != nullptr)
      {
        file->close();
        delete file;
      }
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
    *device->files[index] << device->get_header(",") << "\n" << flush;
}

vector<Device *> Chimera::parse_message(const double& timestamp, const int &id, uint8_t data[], const int &size){
  modifiedDevices.clear();

  switch (id) {
    case 0x4EC:
      gyro->scale = 245;
      gyro->x = (data[0] << 8) + data[1];
      gyro->y = (data[2] << 8) + data[3];
      gyro->z = (data[4] << 8) + data[5];

      if(gyro->x > 32767)
        gyro->x -= 65536;
      if(gyro->y > 32767)
        gyro->y -= 65536;
      if(gyro->z > 32767)
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

      if(accel->x > 32767)
        accel->x -= 65536;
      if(accel->y > 32767)
        accel->y -= 65536;
      if(accel->z > 32767)
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
      // Speed filtered
      else if(data[0] == 0xA8){
        inverter_left->speed = (data[2] << 8) + data[1];
        if(inverter_left->speed > 32767)
          inverter_left->speed -= 65535;

        inverter_left->speed *= (7000.0/32767.0);
        inverter_left->speed *= (6.28318/60.0);

        // Motor reduction ratio
        inverter_left->speed /= 3.47;

        inverter_left->timestamp = timestamp;
        modifiedDevices.push_back(inverter_left);
      }
      // Speed unfiltered
      else if(data[0] == 0x30){
        inverter_left->speed = (data[2] << 8) + data[1];
        if(inverter_left->speed > 32767)
          inverter_left->speed -= 65535;

        inverter_left->speed *= (7000.0/32767.0);
        inverter_left->speed *= (6.28318/60.0);

        // Motor reduction ratio
        inverter_left->speed /= 3.47;

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
      // Speed filtered
      else if(data[0] == 0xA8){
        inverter_right->speed = (data[2] << 8) + data[1];
        if(inverter_right->speed > 32767)
          inverter_right->speed -= 65535;
        inverter_right->speed *= (7000.0/32767.0);
        inverter_right->speed *= (6.28318/60.0);

        // Motor reduction ratio
        inverter_right->speed /= 3.47;

        inverter_right->timestamp = timestamp;
        modifiedDevices.push_back(inverter_right);
      }
      // Speed unfiltered
      else if(data[0] == 0x30){
        inverter_right->speed = (data[2] << 8) + data[1];
        if(inverter_right->speed > 32767)
          inverter_right->speed -= 65535;
        inverter_right->speed *= -1;
        inverter_right->speed *= (7000.0/32767.0);
        inverter_right->speed *= (6.28318/60.0);

        // Motor reduction ratio
        inverter_right->speed /= 3.47;

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
        // if(size < NUM_STATES)
        //   break;
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
    // Temp 1
    case 0x5B0:
      temp_fl->temps[ 0] = (((data[0] << 8) + data[1]) * 0.1) - 100;
      temp_fl->temps[ 1] = (((data[2] << 8) + data[3]) * 0.1) - 100;
      temp_fl->temps[ 2] = (((data[4] << 8) + data[5]) * 0.1) - 100;
      temp_fl->temps[ 3] = (((data[6] << 8) + data[7]) * 0.1) - 100;
      temp_fl->timestamp = timestamp;
      modifiedDevices.push_back(temp_fl);
    break;
    case 0x5B1:
      temp_fl->temps[ 4] = (((data[0] << 8) + data[1]) * 0.1) - 100;
      temp_fl->temps[ 5] = (((data[2] << 8) + data[3]) * 0.1) - 100;
      temp_fl->temps[ 6] = (((data[4] << 8) + data[5]) * 0.1) - 100;
      temp_fl->temps[ 7] = (((data[6] << 8) + data[7]) * 0.1) - 100;
      temp_fl->timestamp = timestamp;
      modifiedDevices.push_back(temp_fl);
    break;
    case 0x5B2:
      temp_fl->temps[ 8] = (((data[0] << 8) + data[1]) * 0.1) - 100;
      temp_fl->temps[ 9] = (((data[2] << 8) + data[3]) * 0.1) - 100;
      temp_fl->temps[10] = (((data[4] << 8) + data[5]) * 0.1) - 100;
      temp_fl->temps[11] = (((data[6] << 8) + data[7]) * 0.1) - 100;
      temp_fl->timestamp = timestamp;
      modifiedDevices.push_back(temp_fl);
    break;
    case 0x5B3:
      temp_fl->temps[12] = (((data[0] << 8) + data[1]) * 0.1) - 100;
      temp_fl->temps[13] = (((data[2] << 8) + data[3]) * 0.1) - 100;
      temp_fl->temps[14] = (((data[4] << 8) + data[5]) * 0.1) - 100;
      temp_fl->temps[15] = (((data[6] << 8) + data[7]) * 0.1) - 100;
      temp_fl->timestamp = timestamp;
      modifiedDevices.push_back(temp_fl);
    break;
    // Temp 2
    case 0x5B4:
      temp_fr->temps[ 0] = (((data[0] << 8) + data[1]) * 0.1) - 100;
      temp_fr->temps[ 1] = (((data[2] << 8) + data[3]) * 0.1) - 100;
      temp_fr->temps[ 2] = (((data[4] << 8) + data[5]) * 0.1) - 100;
      temp_fr->temps[ 3] = (((data[6] << 8) + data[7]) * 0.1) - 100;
      temp_fr->timestamp = timestamp;
      modifiedDevices.push_back(temp_fr);
    break;
    case 0x5B5:
      temp_fr->temps[ 4] = (((data[0] << 8) + data[1]) * 0.1) - 100;
      temp_fr->temps[ 5] = (((data[2] << 8) + data[3]) * 0.1) - 100;
      temp_fr->temps[ 6] = (((data[4] << 8) + data[5]) * 0.1) - 100;
      temp_fr->temps[ 7] = (((data[6] << 8) + data[7]) * 0.1) - 100;
      temp_fr->timestamp = timestamp;
      modifiedDevices.push_back(temp_fr);
    break;
    case 0x5B6:
      temp_fr->temps[ 8] = (((data[0] << 8) + data[1]) * 0.1) - 100;
      temp_fr->temps[ 9] = (((data[2] << 8) + data[3]) * 0.1) - 100;
      temp_fr->temps[10] = (((data[4] << 8) + data[5]) * 0.1) - 100;
      temp_fr->temps[11] = (((data[6] << 8) + data[7]) * 0.1) - 100;
      temp_fr->timestamp = timestamp;
      modifiedDevices.push_back(temp_fr);
    break;
    case 0x5B7:
      temp_fr->temps[12] = (((data[0] << 8) + data[1]) * 0.1) - 100;
      temp_fr->temps[13] = (((data[2] << 8) + data[3]) * 0.1) - 100;
      temp_fr->temps[14] = (((data[4] << 8) + data[5]) * 0.1) - 100;
      temp_fr->temps[15] = (((data[6] << 8) + data[7]) * 0.1) - 100;
      temp_fr->timestamp = timestamp;
      modifiedDevices.push_back(temp_fr);
    break;
    // Temp 3
    case 0x5B8:
      temp_rl->temps[ 0] = (((data[0] << 8) + data[1]) * 0.1) - 100;
      temp_rl->temps[ 1] = (((data[2] << 8) + data[3]) * 0.1) - 100;
      temp_rl->temps[ 2] = (((data[4] << 8) + data[5]) * 0.1) - 100;
      temp_rl->temps[ 3] = (((data[6] << 8) + data[7]) * 0.1) - 100;
      temp_rl->timestamp = timestamp;
      modifiedDevices.push_back(temp_rl);
    break;
    case 0x5B9:
      temp_rl->temps[ 4] = (((data[0] << 8) + data[1]) * 0.1) - 100;
      temp_rl->temps[ 5] = (((data[2] << 8) + data[3]) * 0.1) - 100;
      temp_rl->temps[ 6] = (((data[4] << 8) + data[5]) * 0.1) - 100;
      temp_rl->temps[ 7] = (((data[6] << 8) + data[7]) * 0.1) - 100;
      temp_rl->timestamp = timestamp;
      modifiedDevices.push_back(temp_rl);
    break;
    case 0x5BA:
      temp_rl->temps[ 8] = (((data[0] << 8) + data[1]) * 0.1) - 100;
      temp_rl->temps[ 9] = (((data[2] << 8) + data[3]) * 0.1) - 100;
      temp_rl->temps[10] = (((data[4] << 8) + data[5]) * 0.1) - 100;
      temp_rl->temps[11] = (((data[6] << 8) + data[7]) * 0.1) - 100;
      temp_rl->timestamp = timestamp;
      modifiedDevices.push_back(temp_rl);
    break;
    case 0x5BB:
      temp_rl->temps[12] = (((data[0] << 8) + data[1]) * 0.1) - 100;
      temp_rl->temps[13] = (((data[2] << 8) + data[3]) * 0.1) - 100;
      temp_rl->temps[14] = (((data[4] << 8) + data[5]) * 0.1) - 100;
      temp_rl->temps[15] = (((data[6] << 8) + data[7]) * 0.1) - 100;
      temp_rl->timestamp = timestamp;
      modifiedDevices.push_back(temp_rl);
    break;
    // Temp 4
    case 0x5BC:
      temp_rr->temps[ 0] = (((data[0] << 8) + data[1]) * 0.1) - 100;
      temp_rr->temps[ 1] = (((data[2] << 8) + data[3]) * 0.1) - 100;
      temp_rr->temps[ 2] = (((data[4] << 8) + data[5]) * 0.1) - 100;
      temp_rr->temps[ 3] = (((data[6] << 8) + data[7]) * 0.1) - 100;
      temp_rr->timestamp = timestamp;
      modifiedDevices.push_back(temp_rr);
    break;
    case 0x5BD:
      temp_rr->temps[ 4] = (((data[0] << 8) + data[1]) * 0.1) - 100;
      temp_rr->temps[ 5] = (((data[2] << 8) + data[3]) * 0.1) - 100;
      temp_rr->temps[ 6] = (((data[4] << 8) + data[5]) * 0.1) - 100;
      temp_rr->temps[ 7] = (((data[6] << 8) + data[7]) * 0.1) - 100;
      temp_rr->timestamp = timestamp;
      modifiedDevices.push_back(temp_rr);
    break;
    case 0x5BE:
      temp_rr->temps[ 8] = (((data[0] << 8) + data[1]) * 0.1) - 100;
      temp_rr->temps[ 9] = (((data[2] << 8) + data[3]) * 0.1) - 100;
      temp_rr->temps[10] = (((data[4] << 8) + data[5]) * 0.1) - 100;
      temp_rr->temps[11] = (((data[6] << 8) + data[7]) * 0.1) - 100;
      temp_rr->timestamp = timestamp;
      modifiedDevices.push_back(temp_rr);
    break;
    case 0x5BF:
      temp_rr->temps[12] = (((data[0] << 8) + data[1]) * 0.1) - 100;
      temp_rr->temps[13] = (((data[2] << 8) + data[3]) * 0.1) - 100;
      temp_rr->temps[14] = (((data[4] << 8) + data[5]) * 0.1) - 100;
      temp_rr->temps[15] = (((data[6] << 8) + data[7]) * 0.1) - 100;
      temp_rr->timestamp = timestamp;
      modifiedDevices.push_back(temp_rr);
    break;
    default:
    break;
  }
  return modifiedDevices;
}

int Chimera::parse_gps(Gps* gps_, const double& timestamp, string& line)
{
  if(line[0] != '$')
    return -1;

  vector<string> s_line = split(line, ',');

  if(s_line[0].size() != 6)
    return -2;

  // remove $GP or $GN
  s_line[0].erase(0, 3);

  if (s_line[0] == "GGA") {
    if(s_line.size() != 15)
      return -6;
    // check if needed fileds are empty
    int ret = empty_fields(s_line,vector<int>{1,2,4,6,7,9});
    if(ret != -1)
      return -3;

    gps_->clear();


    gps_->timestamp = timestamp;
    gps_->msg_type = "GGA";

    gps_->time = s_line[1];
    gps_->latitude = stod(s_line[2])/100.0;
    gps_->longitude = stod(s_line[4])/100.0;
    gps_->fix = stoi(s_line[6]);
    gps_->satellites = stoi(s_line[7]);
    gps_->horizontal_diluition_precision = stod(s_line[8]);
    gps_->fix_state = FIX_STATE[gps_->fix];
    gps_->altitude = stod(s_line[9]);
    if(s_line[13] != "")
    {
      gps_->age_of_correction = stod(s_line[13]); 
    }

    return 1;
  }
  else if (s_line[0] == "VTG")
  {
    if(s_line.size() != 10)
      return -5;

    gps_->clear();

    gps_->timestamp = timestamp;
    gps_->msg_type = "VTG";
    
    if(s_line[1] != "")
      gps_->course_over_ground_degrees = stod(s_line[1]);
    else
      gps_->course_over_ground_degrees = 0.0;
    if(s_line[3] != "")
      gps_->course_over_ground_degrees_magnetic = stod(s_line[3]);
    else
      gps_->course_over_ground_degrees_magnetic = 0.0;
    if(s_line[7] != "")
      gps_->speed_kmh = stod(s_line[7]);
    else
      gps_->speed_kmh = 0.0;

    return 1;
  }
  else if(s_line[0] == "GSA")
  {
    if(s_line.size() != 19)
      return -5;

    gps_->clear();

    gps_->timestamp = timestamp;
    gps_->msg_type = "GSA";
    gps_->mode = FIX_MODE[stoi(s_line[2])-1];
    gps_->position_diluition_precision = stod(s_line[15]);
    gps_->horizontal_diluition_precision = stod(s_line[16]);
    gps_->vertical_diluition_precision = stod(s_line[17]);
    return 1;
  }

  return 0;
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

  this->temp_fl->serialize(chimera_proto->add_temp_fl());
  this->temp_fr->serialize(chimera_proto->add_temp_fr());
  this->temp_rl->serialize(chimera_proto->add_temp_rl());
  this->temp_rr->serialize(chimera_proto->add_temp_rr());

  this->gps1->serialize(chimera_proto->add_gps1());
  this->gps2->serialize(chimera_proto->add_gps2());
}


void Chimera::serialize_device(Device* device){
  if (device == accel){
    this->accel->serialize(chimera_proto->add_accel());
  }
  else if(device == gyro)
  {
    this->gyro->serialize(chimera_proto->add_gyro());
  }
  else if(device == encoder_left)
  {
    this->encoder_left->serialize(chimera_proto->add_encoder_left());
  }
  else if(device == encoder_right)
  {
    this->encoder_right->serialize(chimera_proto->add_encoder_right());
  }
  else if(device == inverter_right)
  {
    this->inverter_right->serialize(chimera_proto->add_inverter_right());
  }
  else if(device == inverter_left)
  {
    this->inverter_left->serialize(chimera_proto->add_inverter_left());
  }
  else if(device == bms_lv)
  {
    this->bms_lv->serialize(chimera_proto->add_bms_lv());
  }
  else if(device == bms_hv)
  {
    this->bms_hv->serialize(chimera_proto->add_bms_hv());
  }
  else if(device == pedal)
  {
    this->pedal->serialize(chimera_proto->add_pedal());
  }
  else if(device == steer)
  {
    this->steer->serialize(chimera_proto->add_steer());
  }
  else if(device == ecu_state)
  {
    this->ecu_state->serialize(chimera_proto->add_ecu_state());
  }
  else if(device == bms_hv_state)
  {
    this->bms_hv_state->serialize(chimera_proto->add_bms_hv_state());
  }
  else if(device == steering_wheel_state)
  {
    this->steering_wheel_state->serialize(chimera_proto->add_steering_wheel_state());
  }
  else if(device == ecu)
  {
    this->ecu->serialize(chimera_proto->add_ecu());
  }
  else if(device == temp_fl)
  {
    this->temp_fl->serialize(chimera_proto->add_temp_fl());
  }
  else if(device == temp_fr)
  {
    this->temp_fr->serialize(chimera_proto->add_temp_fr());
  }
  else if(device == temp_rl)
  {
    this->temp_rl->serialize(chimera_proto->add_temp_rl());
  }
  else if(device == temp_rr)
  {
    this->temp_rr->serialize(chimera_proto->add_temp_rr());
  }
  else if(device == gps1)
  {
    this->gps1->serialize(chimera_proto->add_gps1());
  }
  else if(device == gps2)
  {
    this->gps2->serialize(chimera_proto->add_gps2());
  }
}
