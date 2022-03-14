#include "gps_log_player.h"

string BASENAME = "/home/gps";

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
    if(write(fd, shared_string.c_str(), shared_string.size()) == -1)
    {
      close(fd);
      mkfifo(fname.c_str(), 0666);
      fd = open(fname.c_str(), O_WRONLY);
    }
  }
  close(fd);
}

int main(){

  signal(SIGPIPE, SIG_IGN);

  {
    int counter = 0;
    while( remove((BASENAME + to_string(counter)).c_str() ) == 0 ){
      counter ++;
      cout << "Deleted: " << BASENAME << counter << endl;
    }
    cout << "Removed " << counter << " previous ports" << endl;
  }
  string filename = "";
  for (int i = 0; i < 6; i++)
  {
    filename = BASENAME + to_string(i);
    thread *t = new thread(writer, filename);
  }

  Browse b;
  b.SetMaxSelections(1);
  b.SetExtension(".log");
  b.SetSelectionType(SelectionType::sel_all);
  auto selected_paths = b.Start();

  if(selected_paths.size() <= 0){
    cout << "No file selected... exiting" << endl;
    return -1;
  }

  for (auto file : selected_paths){
    cout << file << endl;
    gps_message msg;
    vector<string> lines;
    get_lines(file, &lines);
    double prev_timestamp = -1;
    auto start_time = steady_clock::now();
    for(int i = 20; i < lines.size(); i++){
      if(!parse_gps_line(lines [i], &msg))
        continue;

      {
        std::lock_guard<std::mutex> lk(mtx);
        shared_string = msg.message;
      }
      cv.notify_all();

      if(prev_timestamp > 0){
        usleep((msg.timestamp - prev_timestamp)*1000000);
      }

      prev_timestamp = msg.timestamp;
    }
  }

  return  0;
}


double get_timestamp()
{
  return duration_cast<duration<double, milli>>(system_clock::now().time_since_epoch()).count() / 1000;
}