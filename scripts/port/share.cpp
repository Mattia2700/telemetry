#include <stdio.h>
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>


#include <signal.h>

#include "serial.h"


using namespace std;

std::mutex mtx;
std::condition_variable cv;

string shared_string0;
string shared_string1;

const char * gps0 = "/home/gps0";
const char * gps1 = "/home/gps1";

void GPS0_thread(){
  int fd;
  mkfifo(gps0, 0666);
  fd = open(gps0, O_WRONLY);

  while(1){
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk); // wait for notify by main thread

    // Copy values from the shared string to the fifo file
    write(fd, shared_string0.c_str(), shared_string0.size());
  }
  close(fd);
}

void GPS1_thread(){
  int fd;
  mkfifo(gps1, 0666);
  fd = open(gps1, O_WRONLY);
  while(1){
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk); // wait for notify by main thread

    // Copy values from the shared string to the fifo file
    write(fd, shared_string0.c_str(), shared_string0.size());
  }
  close(fd);
}

int main()
{
  // Ignore when the pipe closes (from a reader that disconnects)
  signal(SIGPIPE, SIG_IGN);

  // serialport
  string port = "/dev/ttyACM0";
  serial s(port);

  if(s.open_port() < 0){
    cout << "Failed opening " << port << endl;
    return -1;
  }

  // Start one thread for each fifo file (pipe)
  thread t0(GPS0_thread);
  thread t1(GPS1_thread);

  string line;
  while (1)
  {
    // Reading line from serialport
    line = s.read_line('\n') + "\n";
    {
      std::lock_guard<std::mutex> lk(mtx);
      shared_string0 = line;
      shared_string1 = line;
    }
    // Notify threads of new data available
    cv.notify_all();
  }
  return 0;
}
