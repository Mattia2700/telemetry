#include "main.h"

#include "can.h"

int main(){

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

  char* data = new char[8];
  data[0] = 10;
  data[1] = 10;
  data[2] = 10;
  can->send(0x10, data, 8);


  return 0;
}