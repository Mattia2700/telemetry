#ifndef SERIAL_H
#define SERIAL_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <filesystem>

#include <string>
#include <iostream>
#include <fstream>

using namespace std;
class serial
{
public:
  serial(string device);
  serial(){};
  ~serial();

  int open_port();
  int open_file();
  void close_port();

  bool file_exists();

  char get_char();
  string read_line(char separator = '\n');

private:
  int fd;
  string device;
};

#endif // SERIAL_H
