#include "log_player.h"

int main(){

  CAN_DEVICE = "can0";
  can = new Can(CAN_DEVICE, &addr);
  int sock = can->open();

  if (sock < 0)
  {
    cout << "Failed binding socket: " << CAN_DEVICE;
    CAN_DEVICE = "vcan0";
    can = new Can(CAN_DEVICE, &addr);
    sock = can->open();
    if (sock < 0)
    {
      cout << "FATAL Failed binding socket: " << CAN_DEVICE;
      return 0;
    }
  }
  cout << "Opened Socket: " << CAN_DEVICE;

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

  for (auto file : selected_paths){
    message msg;
    vector<string> lines;
    get_lines(file, &lines);
    double prev_timestamp = -1;
    auto start_time = steady_clock::now();
    for(int i = 20; i < lines.size(); i++){
      if(!parse_message(lines [i], &msg))
        continue;

      
      can->send(msg.id, (char*)msg.data, msg.size);

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
