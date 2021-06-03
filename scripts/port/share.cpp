#include <stdio.h>
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include <iostream>

#include "serial.h"


using namespace std;
int main()
{
  int fd0;
  int fd1;


  // FIFO file path
  const char * gps0 = "/home/gps0";
  const char * gps1 = "/home/gps1";

  string port = "/dev/ttyACM1";
  serial s(port);

  if(s.open_port() < 0){
    cout << "Faild opening" << endl;
    return -1;
  }
  
  mkfifo(gps0, 0666);
  mkfifo(gps1, 0666);

  int count = 0;

  fd0 = open(gps0, O_WRONLY);
  fd1 = open(gps1, O_WRONLY);
  string line;
  while (1)
  {
    line = to_string(count) + " " + s.read_line('\n') + "\n"; 

    write(fd0, line.c_str(), line.size());
    write(fd1, line.c_str(), line.size());

    count ++;
    cout << count << endl;
  }
  close(fd0);
  close(fd1);
  return 0;
}
