#include "log_player.h"

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

  CAN_DEVICE = "can0";
  can = new Can(CAN_DEVICE, &addr);
  int sock = can->open_socket();

  if (sock < 0)
  {
    cout << "Failed binding socket: " << CAN_DEVICE;
    CAN_DEVICE = "vcan0";
    can = new Can(CAN_DEVICE, &addr);
    sock = can->open_socket();
    if (sock < 0)
    {
      cout << "FATAL Failed binding socket: " << CAN_DEVICE;
      return 0;
    }
  }
  cout << "Opened Socket: " << CAN_DEVICE;

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
  for (int i = 0; i < 1; i++)
  {
    filename = BASENAME + to_string(i);
    thread *t = new thread(writer, filename);
  }
  // {
  //   std::lock_guard<std::mutex> lk(mtx);
  //   shared_string = line;
  // }

  Browse b;
  b.SetMaxSelections(1);
  b.SetExtension(".log");
  b.SetSelectionType(SelectionType::sel_all);
  auto selected_paths = b.Start();

  if(selected_paths.size() <= 0){
    cout << "No file selected... exiting" << endl;
    return -1;
  }

  string folder = std::filesystem::path(selected_paths[0]).parent_path().filename().string();
  cout << selected_paths[0] << endl;

  for (auto file : selected_paths){
    message msg;
    vector<string> lines;
    get_lines(file, &lines);
    while(true)
    {
      double prev_timestamp = -1;
      auto start_time = steady_clock::now();
      for(int i = 20; i < lines.size(); i++){
        if(!parse_message(lines [i], &msg))
          continue;

        
        can->send(msg.id, (char*)msg.data, msg.size);

        if(prev_timestamp > 0){
          usleep((msg.timestamp - prev_timestamp)*100000);
        }

        prev_timestamp = msg.timestamp;
      }
    }
  }

  return  0;
}


double get_timestamp()
{
  return duration_cast<duration<double, milli>>(system_clock::now().time_since_epoch()).count() / 1000;
}