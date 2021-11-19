#include "telemetry.h"

int main(int argc, char** argv)
{
  console = new Debug::Console();

  string HOME_PATH = getenv("HOME");
  string config_fname = HOME_PATH + "/telemetry_config.json";

  console->SaveAllMessages(HOME_PATH + "/telemetry_log.debug");

  load_config(config, config_fname);

  if(!open_log_folder())
    return 0;
  FOLDER_PATH = HOME_PATH + FOLDER_PATH;

  if(!open_can_socket())
    return 0;

  c = new Client();
  c->set_on_message(&on_message);
  auto current_thread = c->run(config.url);
  if(current_thread == nullptr){
    console->ErrorMessage("Failed connecting to server: " + config.url);
  }
  // Login as telemetry
  c->set_data("{\"identifier\":\"telemetry\"}");

  time_t date;
  string human_date;

  // sends status every 200 ms
  thread t(send_status);
  thread ws_thread(send_ws_data);

  chimera = new Chimera();
  GpsLogger* gps = new GpsLogger(string(GPS_DEVICE));
  gps->SetMode(MODE_FILE);
  gps->SetCallback(&on_gps_line);

  GpsLogger* gps2 = new GpsLogger("/dev/ttyACM1");
  gps2->SetOutFName("gps_backup");
  gps2->SetMode(MODE_PORT);

  usleep(100000);
  gps->Start();
  gps2->Start();

  string header;
  string subfolder;
  string folder;
  double timestamp;
  double ws_send_timer = get_timestamp();

  while (true)
  {

    timestamp = get_timestamp();
    can->receive(&message);
    can_stat.msg_count++;

    if(run_state == 0 && start(message))
    {
      // if config are not setted bu can message, then load
      // previous config
      if(config.pilot == 0 && config.circuit == 0 && config.race == 0)
      {
        load_config(config, config_fname);
      }
      else
      {
        write_config(config, config_fname);
      }

      date = time(0);
      human_date = string(ctime(&date));
      human_date.erase(human_date.size()-1, 1);
      console->DebugMessage("Running");

      // Insert header at top of the file
      create_header(header);

      create_folder_name(subfolder);

      // get absolute path of folder
      folder = FOLDER_PATH + "/" + subfolder;
      create_directory(folder);

      gps->SetOutputFolder(folder);
      gps->SetHeader(header);

      gps2->SetOutputFolder(folder);
      gps2->SetHeader(header);

      dump_file = new std::fstream(folder + "/" + "candump.log", std::fstream::out);
      (*dump_file) << header << "\n";

      chimera->add_filenames(folder, ".csv");
      chimera->open_all_files();
      chimera->write_all_headers(0);

      can_stat.duration = timestamp;


      gps->StartLogging();
      gps2->StartLogging();

      run_state = 1;
    }

    modifiedDevices = chimera->parse_message(timestamp, message.can_id, message.data, message.can_dlc);

    if(run_state == 1)
    {
      log_can(timestamp, message, *dump_file);

    }
    for (auto modified : modifiedDevices)
    {
      unique_lock<mutex> lck(mtx);
      if(run_state == 1)
      {
        (*modified->files[0]) << modified->get_string(",") + "\n";
      }
      chimera->serialize_device(modified);
    }

    // Stop message
    if (message.can_id  == 0xA0 && message.can_dlc >= 2 &&
        message.data[0] == 0x66 && message.data[1] == 0x00 &&
        run_state != 0)
    {
      unique_lock<mutex> lck(mtx);

      run_state = false;
      state_changed = true;

      date = time(0);
      human_date = string(ctime(&date));
      human_date[human_date.size()-1] = ' ';
      console->DebugMessage("Stopped");

      chimera->close_all_files();
      dump_file->close();
      delete dump_file;


      // Stop logging but continue reading port
      gps->StopLogging();
      gps2->StopLogging();
      gps->Start();
      gps2->Start();

      can_stat.duration = get_timestamp() - can_stat.duration;
      save_stat(folder);
    }

  }
  return 0;
}

void on_gps_line(string line)
{
  int ret = 0;
  try{
    ret = chimera->parse_gps(get_timestamp(), line);
  }
  catch(std::exception e)
  {
    console->DebugMessage("GPS parse error");
    return;
  }
  if(ret == 1)
  {
    unique_lock<mutex> lck(mtx);
    chimera->serialize_device(chimera->gps);
    if(run_state)
    {
      (*chimera->gps->files[0]) << chimera->gps->get_string(",") + "\n" << flush;
    }
  }
  else
  {
    // cout << ret << " " << line << endl;
  }
}

double get_timestamp()
{
  return duration_cast<duration<double, milli>>(system_clock::now().time_since_epoch()).count() / 1000;
}

string get_hex(int num, int zeros)
{
  stringstream ss;
  ss << setw(zeros) << uppercase << setfill('0') << hex << num;
  return ss.str();
}


void log_can(double& timestamp, can_frame& msg, std::fstream& out)
{
  stringstream line;
  line << "(" << to_string(timestamp) << ")\t" << CAN_DEVICE << "\t";

  // Format message as ID#<payload>
  // Hexadecimal representation
  line << get_hex(int(msg.can_id), 3) << "#";
  for (int i = 0; i < msg.can_dlc; i++)
  {
    line << get_hex(int(msg.data[i]), 2);
  }

  line << "\n";

  out << line.str();
}


int open_log_folder()
{
  HOME_PATH = getenv("HOME");
  FOLDER_PATH = "/logs";
  console->DebugMessage("Output Folder " + HOME_PATH + FOLDER_PATH);
  if (!path_exists(HOME_PATH + FOLDER_PATH))
  {
    console->WarnMessage("Failed, changing folder... ");
    FOLDER_PATH = "/Desktop/logs";
    console->DebugMessage("Output Folder " + HOME_PATH + FOLDER_PATH);
    if (!path_exists(HOME_PATH + FOLDER_PATH))
    {
      console->ErrorMessage("Folder not found!" + FOLDER_PATH);
      return 0;
    }
  }
  return 1;
}

int open_can_socket()
{
  can = new Can(CAN_DEVICE, &addr);
  int sock = can->open();

  if (sock < 0)
  {
    console->WarnMessage("Failed binding socket: " + string(CAN_DEVICE));
    CAN_DEVICE = "vcan0";
    can = new Can(CAN_DEVICE, &addr);
    sock = can->open();
    if (sock < 0)
    {
      console->ErrorMessage("FATAL Failed binding socket: " + string(CAN_DEVICE));
      return 0;
    }
  }
  console->DebugMessage("Opened Socket: " + string(CAN_DEVICE));
  return 1;
}

int start(const can_frame& message)
{
  if (message.can_id  == 0xA0 && message.can_dlc >= 2 &&
      message.data[0] == 0x66 && message.data[1] == 0x01)
  {
    // If contains some payload use to setup pilots races and circuits
    if (message.can_dlc >= 5)
    {
      config.pilot = message.data[2];
      config.race = message.data[3];
      config.circuit = message.data[4];
    }
    else
    {
      config.pilot = 0;
      config.race = 0;
      config.circuit = 0;
    }
    return 1;
    // Check index range
    if (config.pilot >= PILOTS.size())
    config.pilot = 0;
    if (config.race >= RACES.size())
    config.race = 0;
    if (config.circuit >= CIRCUITS.size())
    config.circuit = 0;
  }
  return 0;
}

void create_header(string& out)
{
  // Get human readable date
  time_t date = time(0);
  string human_date = string(ctime(&date));
  out = "\r\n\n";
  out += "*** EAGLE-TRT\r\n";
  out += "*** Telemetry Log File\r\n";
  out += "*** " + human_date;
  out += "\r\n";
  out += "*** Pilot   .... " + PILOTS[config.pilot] + "\r\n";
  out += "*** Race    .... " + RACES[config.race] + "\r\n";
  out += "*** Circuit .... " + CIRCUITS[config.circuit];
  out += "\n\n\r";
}

void create_folder_name(string& out)
{
  time_t date = time(0);
  tm *ltm = localtime(&date);
  // Create a unique folder
  stringstream subfolder;
  subfolder << to_string(1900 + ltm->tm_year);
  subfolder << setw(2) << setfill('0') << to_string(1 + ltm->tm_mon);
  subfolder << setw(2) << setfill('0') << to_string(ltm->tm_mday);
  subfolder << "_";
  subfolder << setw(2) << setfill('0') << to_string(ltm->tm_hour);
  subfolder << setw(2) << setfill('0') << to_string(ltm->tm_min);
  subfolder << setw(2) << setfill('0') << to_string(ltm->tm_sec);
  subfolder << "_";
  subfolder << PILOTS[config.pilot];
  subfolder << "_";
  subfolder << RACES[config.race];

  out = subfolder.str();
}

void send_status()
{
  while(true)
  {
    msg_data[0] = run_state;
    can->send(0x99, (char *)msg_data, 1);
    usleep(420000);
  }
}

void load_config(run_config& cfg, string& path)
{
  if(path_exists(path) == false)
  {
    cfg.circuit = 0;
    cfg.pilot = 0;
    cfg.race = 0;
    config.url = "ws://192.168.195.1:9090";
    write_config(cfg, path);
  }
  else
  {
    stringstream buffer;
    std::ifstream cfg_file(path);
    buffer << cfg_file.rdbuf();
    cfg_file.close();
    string str_buffer = buffer.str();

    Document d;
    StringBuffer sb;
    Writer<StringBuffer> w(sb);
    rapidjson::Document::AllocatorType &alloc = d.GetAllocator();

    d.Parse(str_buffer.c_str(), str_buffer.size());
    const Value& tel_value = d["Telemetry"];
    for(int i = 0; i < CIRCUITS.size(); i++)
      if(CIRCUITS[i] == tel_value["Circuit"].GetString())
        cfg.circuit = i;
    for(int i = 0; i < PILOTS.size(); i++)
      if(PILOTS[i] == tel_value["Pilot"].GetString())
        cfg.pilot = i;
    for(int i = 0; i < RACES.size(); i++)
      if(RACES[i] == tel_value["Race"].GetString())
        cfg.race = i;
    cfg.url = tel_value["URL"].GetString();
  }
}

void write_config(run_config& cfg, string& path)
{
  Document doc;
  rapidjson::Document::AllocatorType &alloc = doc.GetAllocator();
  doc.SetObject();

  // Add subobject
  Value sub_obj;
  sub_obj.SetObject();
  {
    sub_obj.AddMember("Circuit",Value().
    SetString(CIRCUITS[cfg.circuit].c_str(),
              CIRCUITS[cfg.circuit].size(),
              alloc),
              alloc);
    sub_obj.AddMember("Pilot",Value().
    SetString(PILOTS[cfg.pilot].c_str(),
              PILOTS[cfg.pilot].size(),
              alloc),
              alloc);
    sub_obj.AddMember("Race",Value().
    SetString(RACES[cfg.race].c_str(),
              RACES[cfg.race].size(),
              alloc),
              alloc);
    sub_obj.AddMember("URL",Value().
    SetString(cfg.url.c_str(),
              cfg.url.size(),
              alloc),
              alloc);
  }
  doc.AddMember("Telemetry", sub_obj, alloc);

  StringBuffer out_buffer;
  PrettyWriter<StringBuffer> writer(out_buffer);

  doc.Accept(writer);
  std::ofstream cfg_file(path);
  cfg_file << out_buffer.GetString();
  cfg_file.close();
}

void save_stat(string folder)
{
  Document doc;
  StringBuffer json_ss;
  PrettyWriter<StringBuffer> writer(json_ss);
  rapidjson::Document::AllocatorType &alloc = doc.GetAllocator();

  doc.SetObject();
  time_t date = time(0);
  string human_date = string(ctime(&date));
  human_date.erase(human_date.size()-1, 1);
  // Add keys and string values
  doc.AddMember("Date", Value().SetString(StringRef(human_date.c_str())), alloc);
  doc.AddMember("Pilot", Value().SetString(StringRef(PILOTS[config.pilot].c_str())), alloc);
  doc.AddMember("Race", Value().SetString(StringRef(RACES[config.race].c_str())), alloc);
  doc.AddMember("Circuit", Value().SetString(StringRef(CIRCUITS[config.circuit].c_str())), alloc);

  Value val;
  val.SetObject();
  {
    val.AddMember("Messages", can_stat.msg_count, alloc);
    val.AddMember("Average Frequency (Hz)", int(double(can_stat.msg_count) / can_stat.duration), alloc);
    val.AddMember("Duration (seconds)", can_stat.duration, alloc);
  }
  doc.AddMember("CAN", val, alloc);

  doc.Accept(writer);
  std::ofstream stat_f(folder + "/CAN_Info.json");
  stat_f << json_ss.GetString();
  stat_f.close();
}

void send_ws_data()
{
  usleep(10000000);
  while(true)
  {
    usleep(1000000 * TIMEOUT);
    unique_lock<mutex> lck(mtx);
    string serialized_string;
    chimera->serialized_to_string(&serialized_string);

    if(serialized_string.size() == 0)
    {
      continue;
    }

    Document d;
    StringBuffer sb;
    Writer<StringBuffer> w(sb);
    rapidjson::Document::AllocatorType &alloc = d.GetAllocator();

    d.SetObject();
    d.AddMember("type", Value().SetString("update_data"), alloc);
    d.AddMember("timestamp", get_timestamp(), alloc);
    d.AddMember("data", Value().SetString(serialized_string.c_str(), serialized_string.size(), alloc), alloc);
    d.Accept(w);

    c->set_data(sb.GetString());
    chimera->clear_serialized();
  }
}



void on_message(client* cli, websocketpp::connection_hdl hdl, message_ptr msg){
  Document d;
  StringBuffer sb;
  Writer<StringBuffer> w(sb);
  rapidjson::Document::AllocatorType &alloc = d.GetAllocator();

  ParseResult ok = d.Parse(msg->get_payload().c_str(), msg->get_payload().size());
  if(!ok)
  {
    return;
  }
  if(d["type"] == "set_telemetry_config"){
    if(d["data"].HasMember("pilot") &&
      d["data"].HasMember("circuit") &&
      d["data"].HasMember("race"))
    {
      config.pilot = d["data"]["pilot"].GetInt();
      config.circuit = d["data"]["circuit"].GetInt();
      config.race = d["data"]["race"].GetInt();

      cout << "Writing config" << endl;
      string path = HOME_PATH + "/telemetry_config.json";
      write_config(config, path);
      cout << "Done" << endl;
    }
    else{
      cout << "Wrong members" << endl;
    }
  }
}
