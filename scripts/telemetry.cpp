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

  string fname = get_last_fname(FOLDER_PATH);

  sockaddr_can addr;
  Can* can = new Can(CAN_DEVICE, &addr);
  int sock = can->open();

  if(sock == -1){
    cout << "Failed creating socket" << endl;
    return -1;
  }
  else if(sock == -2){
    cout << "Failed bind socket" << endl;
  }

  while(true){

    // TODO: Add can filter
    while(true){
      can->receive(&message);

      if(message.can_id == 0xA0 && message.can_dlc >= 2){
        if(message.data[0] = 0x65 && message.data[1] == 0x01){
          // Start Telemetry
          cout << "Started" << endl;
          break;
          // Add pilots config
        }
      }
    }
    // TODO: remove filter

    string fname = get_last_fname(FOLDER_PATH);
    std::ofstream log(fname);


    stringstream line;
    while(true){
      can->receive(&message);
      
      line.str("");
      line << "(";
      line << to_string(get_timestamp());
      line << ")\t";
      line << CAN_DEVICE << "\t";

      line << get_hex(int(message.can_id), 3) << "#";
      for(int i = 0; i < message.can_dlc; i++){
        line << get_hex(int(message.data[i]), 2);
      }

      log << line.str() << endl;

      if(message.can_id == 0xA0 && message.can_dlc >= 2){
        if(message.data[0] == 0x65 && message.data[1] == 0x00){
          // Stop Telemetry
          cout << "Stopped" << endl;
          log.close();
          break;
        }
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
  return duration_cast<duration<double, milli>>(system_clock::now().time_since_epoch()).count();
}

string get_hex(int num, int zeros){
  stringstream ss;
  ss << setw(zeros) << uppercase << setfill('0') << hex << num; 
  return ss.str();
}