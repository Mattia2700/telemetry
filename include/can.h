#ifndef CAN_H
#define CAN_H

#include <stdio.h>
#include <string.h>
#include <iostream>

#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

using namespace std;
class Can{
public:
  Can(){};
  Can(const char* device, struct sockaddr_can* address);

  int open();
  int send(int id, char* data, int len);
  int receive(can_frame*);

  int set_filters(can_filter& filter);

private:
  int sock;
  const char* device;
  struct sockaddr_can* address;

};

#endif