#include "dashboard.h"

int main(){

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

  devices::Chimera chimera_proto;

  for (auto file : selected_paths){
    message msg;
    vector<string> lines;
    get_lines(file, &lines);
    vector<Device*> modifiedDevices;
    double prev_timestamp = -1;
    auto start_time = steady_clock::now();
    for(int i = 20; i < lines.size(); i++){
      if(!parse_message(lines [i], &msg))
        continue;

      modifiedDevices = chimera.parse_message(msg.timestamp, msg.id, msg.data, msg.size);

      for(auto device : modifiedDevices){

        chimera.serialize_device(device);
        // chimera.serialize_to_string(&serialized_string);
        // chimera.serialize_to_text(&serialized_string);
        // chimera.serialize_to_json(&serialized_string);
      }

      if(prev_timestamp > 0){
        usleep((msg.timestamp - prev_timestamp)*1000000);
      }

      prev_timestamp = msg.timestamp;

      if(duration_cast<duration<double, milli>>(steady_clock::now() - start_time).count() > TIMEOUT){
        start_time = steady_clock::now();
        chimera.serialized_to_string(&serialized_string);

        if(serialized_string.size() == 0)
          continue;

        sb.Clear();
        w.Reset(sb);
        d.SetObject();
        d.AddMember("type", Value().SetString("update_data"), alloc);
        d.AddMember("timestamp", get_timestamp(), alloc);
        d.AddMember("data", Value().SetString(serialized_string.c_str(), serialized_string.size(), alloc), alloc);
        d.Accept(w);

        c.set_data(sb.GetString());
        chimera.clear_serialized();
      }
    }
    chimera.serialized_to_string(&serialized_string);

    if(serialized_string.size() == 0)
      continue;
    sb.Clear();
    w.Reset(sb);
    d.SetObject();
    d.AddMember("type", Value().SetString("update_data"), alloc);
    d.AddMember("timestamp", get_timestamp(), alloc);
    d.AddMember("data", Value().SetString(serialized_string.c_str(), serialized_string.size(), alloc), alloc);
    d.Accept(w);

    c.set_data(sb.GetString());
    chimera.clear_serialized();
  }

  return  0;
}


double get_timestamp()
{
  return duration_cast<duration<double, milli>>(system_clock::now().time_since_epoch()).count() / 1000;
}
