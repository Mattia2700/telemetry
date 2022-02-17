#include "dashboard_can.h"

int main(){

  sockaddr_can addr;
  Can *can = new Can(CAN_DEVICE, &addr);
  int sock = can->open();
  if (sock < 0)
  {
    cout << get_colored("Failed creating/binding socket: " + string(CAN_DEVICE), 1) << endl;
    CAN_DEVICE = "can0";
    can = new Can(CAN_DEVICE, &addr);
    sock = can->open();
    if (sock < 0)
    {
      cout << get_colored("Failed creating/binding socket: " + string(CAN_DEVICE), 1) << endl;
      cout << "Exiting" << endl;
      return 0;
    }
  }
  cout << get_colored("Opened Socket: " + string(CAN_DEVICE), 6) << endl;

  WebSocketClient c;
  auto current_thread = c.run(uri);
  if(current_thread == nullptr){
    cout << get_colored("Failed connecting to server: " + uri, 1) << endl;
    cout << "Exiting" << endl;
    return -1;
  }
  d.Accept(w);
  // Login as telemetry
  c.set_data("{\"identifier\":\"telemetry\"}");

  can_frame message;
  vector<Device*> modifiedDevices;
  double prev_timestamp = -1;
  auto start_time = steady_clock::now();
  double timestamp = get_timestamp();
  while (true)
  {
    timestamp = get_timestamp();
    can->receive(&message);
    modifiedDevices = chimera.parse_message(timestamp, message.can_id, message.data, message.can_dlc);

    for(auto device : modifiedDevices){
      chimera.serialize_device(device);
      // chimera.serialized_to_string(&serialized_string);
      // chimera.serialized_to_text(&serialized_string);
      // chimera.serialized_to_json(&serialized_string);
    }

    if(duration_cast<duration<double, milli>>(steady_clock::now() - start_time).count() > TIMEOUT){
      start_time = steady_clock::now();
      chimera.serialized_to_string(&serialized_string);

      if(serialized_string.size() == 0)
        continue;

      sb.Clear();
      w.Reset(sb);
      d.SetObject();
      d.AddMember("type", Value().SetString("update_data"), alloc);
      d.AddMember("data", Value().SetString(serialized_string.c_str(), serialized_string.size(), alloc), alloc);
      d.Accept(w);

      c.set_data(sb.GetString());
      chimera.clear_serialized();
    }
  }
  return  0;
}


double get_timestamp()
{
  return duration_cast<duration<double, milli>>(system_clock::now().time_since_epoch()).count() / 1000;
}
