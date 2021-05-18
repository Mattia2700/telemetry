#ifndef SERIAL_H
#define SERIAL_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include <string>
#include <iostream>

using namespace std;
class serial{
public:
  serial(string device);
  serial(){};
  ~serial();

  int open_port();
  void close_port();

  string read_line(char separator='\n');

private:
  int fd;
  string device;


};

#endif //SERIAL_H
