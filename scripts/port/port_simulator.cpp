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

#include <cstdio>

#include "utils.h"
#include "serial.h"

using namespace std;
std::mutex mtx;
std::condition_variable cv;

string shared_string;

int N = 6;
string BASENAME = "/home/gps";

vector<string> split(string str, char separator){
  vector<string> ret;
  string bff = "";
  for(int i = 0; i < str.size(); i ++){
    if(str[i] == separator){
      ret.push_back(bff);
      bff = "";
    }
    else{
      bff += str[i];
    }
  }
  ret.push_back(bff);
  return ret;
}


void writer(string fname)
{
  int fd;
  mkfifo(fname.c_str(), 0666);
  fd = open(fname.c_str(), O_WRONLY);
  while (1)
  {
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk); // wait for notify by main thread

    // Copy values from the shared string to the fifo file
    write(fd, shared_string.c_str(), shared_string.size());
  }
  close(fd);
}

int main()
{
  // Ignore when the pipe closes (from a reader that disconnects)
  signal(SIGPIPE, SIG_IGN);

  {
    int counter = 0;
    while( remove((BASENAME + to_string(counter)).c_str() ) == 0 ){
      counter ++;
      cout << "Deleted: " << BASENAME << counter << endl;
    }
    cout << "Removed " << counter << " previous ports" << endl;
  }

  // Start one thread for each fifo file (pipe)
  string filename = "";
  for (int i = 0; i < N; i++)
  {
    filename = BASENAME + to_string(i);
    thread *t = new thread(writer, filename);
  }

  string line;
  stringstream s;
  std::ifstream file("/home/filippo/gps_logger.log");
  s << file.rdbuf();
  vector<string> lines = split(s.str(), '\n');

  while (1)
  {
    for(const string& line : lines)
    {
      {
        std::lock_guard<std::mutex> lk(mtx);
        shared_string = line + "\n";
      }
      // Notify threads of new data available
      cv.notify_all();
      usleep(1000);
    }
  }
  return 0;
}
