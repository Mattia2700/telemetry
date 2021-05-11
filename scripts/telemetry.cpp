#include "telemetry.h"

int main(){
  HOME_PATH = getenv("HOME");
  FOLDER_PATH = "/Desktop/logs";

  FOLDER_PATH = HOME_PATH + FOLDER_PATH;

  if(!exists(FOLDER_PATH)){
    cout << "Folder not found!" << endl;
    cout << FOLDER_PATH << endl;
    return 0;
  }

  sockaddr_can addr;
  Can* can = new Can(CAN_DEVICE, &addr);
  int sock = can->open();

  if(sock < 0){
    cout << "Failed creating/binding socket: " << CAN_DEVICE << endl;
    CAN_DEVICE = "can0";
    can = new Can(CAN_DEVICE, &addr);
    sock = can->open();
    if(sock < 0){
      cout << "Failed creating/binding socket: " << CAN_DEVICE << endl;
      cout << "Exiting" << endl;
      return 0;
    }
  }
  cout << "Opened Socket: " << CAN_DEVICE << endl;

  CIRCUITS = vector<string>({
    "default",
    "Vadena",
    "Varano",
    "Povo",
    "Skio",
  });
  PILOTS = vector<string>({
    "default",
    "Ivan",
    "Filippo",
    "Mirco",
    "Nicola",
    "Davide",
  });
  RACES = vector<string>({
    "default",
    "Autocross",
    "Skidpad",
    "Endurance",
    "Acceleration",
  });

  // indices for PILOTS, RACES, CIRCUITS
  int i1 = 0, i2 = 0, i3 = 0;
  while(true){
    // Send telemetry status IDLE
    msg_data[0] = 0;
    can->send(0x99,(char* )msg_data, 1);

    // Set CAN filter to accept only 0xA0 id
    rfilter.can_id   = 0xA0;
    rfilter.can_mask = 0b11111111111;
    can->set_filters(rfilter);

    while(true){
      can->receive(&message);
      if(message.can_id == 0xA0 && message.can_dlc >= 2){
        // Start Message
        if(message.data[0] = 0x65 && message.data[1] == 0x01){
          cout << "Started" << endl;
          // If contains some payload use to setup pilots races and circuits
          if(message.can_dlc >= 5){
            i1 = message.data[2];
            i2 = message.data[3];
            i3 = message.data[4];
          }else{
            i1 = 0;
            i2 = 0;
            i3 = 0;
          }
          break;
        }
      }
    }
    // To remove a filter set the mask to 0
    // So every id will pass the mask
    rfilter.can_id   = 0x000;
    rfilter.can_mask = 0b00000000000;
    can->set_filters(rfilter);

    time_t date = time(0);
    tm *ltm = localtime(&date);
    // Create a unique folder
    string subfolder = "";
    subfolder += to_string(1900 + ltm->tm_year);
    subfolder += to_string(1    + ltm->tm_mon);
    subfolder += to_string(       ltm->tm_mday);
    subfolder += "_";
    subfolder += to_string(ltm->tm_hour);
    subfolder += to_string(ltm->tm_min);
    subfolder += to_string(ltm->tm_sec);
    subfolder += "_";
    subfolder += PILOTS[i1];
    subfolder += "_";
    subfolder += RACES[i2];

    create_directory(FOLDER_PATH + "/" + subfolder);
    string fname = "";
    fname += FOLDER_PATH + "/";
    fname += subfolder + "/";
    fname += "candump.log";
    std::ofstream log(fname);

    // Check index range
    if (i1 >= PILOTS.size())
      i1 = 0;
    if (i2 >= RACES.size())
      i2 = 0;
    if (i3 >= CIRCUITS.size())
      i3 = 0;

    // Get human readable date
    char* date_c = ctime(&date);

    // Insert header at top of the file
    log << "\r\n\n"
        "*** EAGLE-TRT\r\n"
        "*** Telemetry Log File\r\n"
        "*** " << date_c <<
        "\r\n"
        "*** Pilot: " << PILOTS[i1] << "\r\n"
        "*** Race: " << RACES[i2] << "\r\n"
        "*** Circuit: " << CIRCUITS[i3] <<
        "\n\n\r";

    stringstream line;

    // Use this timer to send status messages
    auto t_start = high_resolution_clock::now();
    auto t_end = high_resolution_clock::now();
    while(true){
      can->receive(&message);

      line.str("");
      line << "(";
      line << to_string(get_timestamp());
      line << ")\t";
      line << CAN_DEVICE << "\t";

      // Format message as ID#<payload>
      // Hexadecimal representation
      line << get_hex(int(message.can_id), 3) << "#";
      for(int i = 0; i < message.can_dlc; i++){
        line << get_hex(int(message.data[i]), 2);
      }

      // Write in file
      log << line.str() << endl;

      if(message.can_id == 0xA0 && message.can_dlc >= 2){
        // Stop message
        if(message.data[0] == 0x65 && message.data[1] == 0x00){
          cout << "Stopped" << endl;
          log.close();
          break;
        }
      }

      t_end = high_resolution_clock::now();
      if(duration<double, milli>(t_end - t_start).count() > 200){
        // Send telemetry status (RUN)
        msg_data[0] = 1;
        can->send(0x99,(char* )msg_data, 1);
        t_start = high_resolution_clock::now();
      }
    }
  }

  return 0;
}

string get_last_fname(string path){
  int number = 0;
  while(exists(path+"/"+to_string(number)+".log")){
    number ++;
  }
  return path+"/"+to_string(number)+".log";
}

double get_timestamp(){
  return duration_cast<duration<double, milli>>(system_clock::now().time_since_epoch()).count()/1000;
}

string get_hex(int num, int zeros){
  stringstream ss;
  ss << setw(zeros) << uppercase << setfill('0') << hex << num;
  return ss.str();
}
