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
  Can* can = new Can("vcan0", &addr);
  int sock = can->open();

  if(sock == -1){
    cout << "Failed creating socket" << endl;
    return -1;
  }
  else if(sock == -2){
    cout << "Failed bind socket" << endl;
  }

  while(true){
    while(true){
      can->receive(&message);

      if(message.can_id == 0xA0 && message.can_dlc >= 2){
        if(message.data[0] = 0x65 && message.data[1] == 0x01){
          cout << "started" << endl;
          // Start Telemetry
        }

        break;
      }
    }

    while(true){
      can->receive(&message);
      cout << id << endl;
      for(int i = 0; i < message.can_dlc; i++){
        cout << to_string(message.data[i]);
      }
      cout << endl;
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
