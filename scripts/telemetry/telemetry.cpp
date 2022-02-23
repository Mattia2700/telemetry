#include "telemetry.h"

int main(int argc, char** argv)
{
  console = new Debug::Console();

  HOME_PATH = getenv("HOME");


  load_all_config(HOME_PATH);

  CAN_DEVICE = tel_conf.can_device.c_str();

  console->SaveAllMessages(HOME_PATH + "/telemetry_log.debug");

  // Opening Log folders
  // /FOLDER_PATH/<date>/<session>
  if(!open_log_folder())
    return 0;

  time(&date);
  localtime_r(&date, &ltm);
  std::ostringstream ss;
  ss << std::put_time(&ltm, "%d_%m_%Y");

  FOLDER_PATH += "/" + ss.str();
  
  FOLDER_PATH = HOME_PATH + FOLDER_PATH;
  if(!path_exists(FOLDER_PATH))
    create_directory(FOLDER_PATH);

  if(!open_can_socket())
    return 0;

  ws_cli = new WebSocketClient();
  ws_conn_thread = new thread(connect_ws);


  chimera = new Chimera();

  // Setup of all GPS devices
  for(size_t i = 0; i < tel_conf.gps_devices.size(); i++)
  {
    string dev = tel_conf.gps_devices[i];
    string mode = tel_conf.gps_mode[i];
    bool enabled = tel_conf.gps_enabled[i];
    
    GpsLogger* gps1 = new GpsLogger(dev);
    gps1->SetOutFName("gps_" + to_string(i+1));
    if(mode == "file")
      gps1->SetMode(MODE_FILE);
    else
      gps1->SetMode(MODE_PORT);
    gps1->SetCallback(&on_gps_line);

    gps_loggers.push_back(gps1);
  }

  usleep(100000);
  for(auto logger : gps_loggers)
    logger->Start();

  string header;
  string subfolder;
  string folder;
  double timestamp;
  double ws_send_timer = get_timestamp();

  while (true)
  {

    can->receive(&message);
    timestamp = get_timestamp();
    can_stat.msg_count++;

    if(run_state == 0 && ((message.can_id  == 0xA0 && message.can_dlc >= 2 &&
      message.data[0] == 0x66 && message.data[1] == 0x01) || ws_reqeust_on))
    {
      ws_reqeust_on = false;
      console->DebugMessage("Received request to run");
      // Insert header at top of the file
      create_header(header);
      
      create_folder_name(subfolder);

      // get absolute path of folder
      int i = 1;
      do{
        folder = FOLDER_PATH + "/" + subfolder + " " + to_string(i);
        i++;
      }while(path_exists(folder));
      create_directory(folder);

      for(auto logger : gps_loggers)
      {
        logger->SetOutputFolder(folder);
        logger->SetHeader(header);
      }

      dump_file = new std::fstream(folder + "/" + "candump.log", std::fstream::out);
      (*dump_file) << header << "\n";

      chimera->add_filenames(folder, ".csv");
      chimera->open_all_files();
      chimera->write_all_headers(0);

      can_stat.duration = timestamp;

      for(auto logger : gps_loggers)
        logger->StartLogging();

      run_state = 1;
      console->DebugMessage("\tRunning");
    }

    if(run_state == 1)
    {
      log_can(timestamp, message, *dump_file);
    }

    // Parse the message only if is needed
    // Parsed messages are for sending via websocket or to be logged in csv
    if(tel_conf.generate_csv || tel_conf.ws_enabled)
    {
      try{
        modifiedDevices = chimera->parse_message(timestamp, message.can_id, message.data, message.can_dlc);
      }
      catch(std::exception ex)
      {
        console->ErrorMessage("Exception when parsing message");
        stringstream ss;
        ss << timestamp << " ";

        ss << get_hex(int(message.can_id), 3) << "#";
        for (int i = 0; i < message.can_dlc; i++)
          ss << get_hex(int(message.data[i]), 2);

        console->ErrorMessage("Message: " + ss.str());
        ss.str("");
        ss << "Exception: " << ex.what();
        console->ErrorMessage(ss.str());
        continue;
      }

      // For every device that has been modified by the parse operation
      for (auto modified : modifiedDevices)
      {
        unique_lock<mutex> lck(mtx);

        if(run_state == 1 && tel_conf.generate_csv)
        {
          (*modified->files[0]) << modified->get_string(",") + "\n";
        }

        // Serialize with protobuf if websocket is enabled
        if(tel_conf.ws_enabled)
        {
          if(tel_conf.ws_downsample == true)
          {
            if((1.0/tel_conf.ws_downsample_mps) < (timestamp - modified->prev_timestamp))
            {
              modified->prev_timestamp = timestamp;
              chimera->serialize_device(modified);
            }
          }else
          {
            chimera->serialize_device(modified);
          }
        }
      }
    }

    // Stop message
    if (run_state == 1 && ((message.can_id  == 0xA0 && message.can_dlc >= 2 &&
        message.data[0] == 0x66 && message.data[1] == 0x00) || ws_reqeust_off))
    {
      ws_reqeust_off = false;
      unique_lock<mutex> lck(mtx);
      console->DebugMessage("Received request to stop");

      // Stop
      run_state.store(0);
      state_changed = true;

      // duration of the log
      can_stat.duration = get_timestamp() - can_stat.duration;

      // Close all csv files and the dump file
      chimera->close_all_files();
      dump_file->close();
      delete dump_file;

      // Stop logging but continue reading port
      for(auto logger : gps_loggers)
      {
        logger->StopLogging();
        logger->Start();
      }

      // Save stats of this log session
      save_stat(folder);
      console->DebugMessage("\tStopped");
    }

  }
  return 0;
}

// Callback, fires every time a line from a GPS is received
void on_gps_line(int id, string line)
{
  Gps* gps = nullptr;

  // Selecting one of chimera GPS
  if(id == 0)
    gps = chimera->gps1;
  else if(id == 1)
    gps = chimera->gps2;

  // Parsing GPS data
  int ret = 0;
  try{
    ret = chimera->parse_gps(gps, get_timestamp(), line);
  }
  catch(std::exception e)
  {
    console->DebugMessage("GPS parse error");
    return;
  }

  // If parsing was successfull
  // save parsed data into gps file
  if(ret == 1)
  {
    unique_lock<mutex> lck(mtx);
    chimera->serialize_device(gps);
    if(run_state)
    {
      (*gps->files[0]) << gps->get_string(",") + "\n" << flush;
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

// Check if log folder exist
// Two folders are checked in sequence:
// "/<home>/logs"
// "/<home>/Desktop/logs"
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

void create_header(string& out)
{
  out = "\r\n\n";
  out += "*** EAGLE-TRT\r\n";
  out += "*** Telemetry Log File\r\n";
  out += "*** Date: " + sesh_config.date + "\r\n";
  out += "*** Time: " + sesh_config.time + "\r\n";
  out += "\r\n";
  out += "*** Curcuit       .... " + sesh_config.circuit + "\r\n";
  out += "*** Pilot         .... " + sesh_config.pilot + "\r\n";
  out += "*** Test          .... " + sesh_config.test + "\r\n";
  out += "*** Configuration .... " + sesh_config.configuration;
  out += "\n\n\r";
}

void create_folder_name(string& out)
{ 
  // Create a folder with current configurations
  stringstream subfolder;
  subfolder << sesh_config.test;
  subfolder << " [";
  subfolder << sesh_config.configuration << "]";

  string s = subfolder.str();
  std::replace(s.begin(), s.end(), '\\', ' ');
  std::replace(s.begin(), s.end(), '/', ' ');

  out = s;
}

// Send status both via can and websocket
void send_status()
{
  while(true)
  {
    msg_data[0] = run_state;
    can->send(0x99, (char *)msg_data, 1);

    if(tel_conf.ws_enabled && ws_conn_state == ConnectionState_::CONNECTED)
    {
      Document d;
      StringBuffer sb;
      Writer<StringBuffer> w(sb);
      rapidjson::Document::AllocatorType &alloc = d.GetAllocator();

      d.SetObject();
      d.AddMember("type", Value().SetString("telemetry_status"), alloc);
      d.AddMember("timestamp", get_timestamp(), alloc);
      d.AddMember("data", run_state.load(), alloc);
      d.Accept(w);

      ws_cli->set_data(sb.GetString());
    }


    usleep(1000000);
  }
}

// Load all configuration files
// If the file doesn't exist create it
void load_all_config(std::string& home_path)
{
  string path = home_path + "/telemetry_config.json";
  if(path_exists(path))
  {
    if(LoadJson(tel_conf, path))
      console->DebugMessage("Loaded telemetry config");
    else
      console->ErrorMessage("Failed loading telemetry config");
  }else{
    console->DebugMessage("Created: " + path);
    SaveJson(tel_conf, path);
  }

  path = home_path + "/session_config.json";
  if(path_exists(path))
  {
    if(LoadJson(sesh_config, path))
      console->DebugMessage("Loaded session config");
    else
      console->ErrorMessage("Failed loading session config");
  }else{
    console->DebugMessage("Created: " + path);
    SaveJson(sesh_config, path);
  }
}

void save_all_config()
{
  string path = " ";
  path = HOME_PATH + "/telemetry_config.json";
  console->DebugMessage("Saving new tel config");
  SaveJson(tel_conf, path);
  console->DebugMessage("Saving done");
  console->DebugMessage("Saving new sesh config");
  path = HOME_PATH + "/session_config.json";
  SaveJson(sesh_config, path);
  console->DebugMessage("Saving done");
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
  doc.AddMember("Circuit", Value().SetString(StringRef(sesh_config.circuit.c_str())), alloc);
  doc.AddMember("Pilot", Value().SetString(StringRef(sesh_config.pilot.c_str())), alloc);
  doc.AddMember("Test", Value().SetString(StringRef(sesh_config.test.c_str())), alloc);
  doc.AddMember("Configuration", Value().SetString(StringRef(sesh_config.configuration.c_str())), alloc);

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
  while(true)
  {
    while(ws_conn_state != ConnectionState_::CONNECTED)
      usleep(500000);
    while(ws_conn_state == ConnectionState_::CONNECTED)
    {
      usleep(1000 * tel_conf.ws_send_rate);

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

      ws_cli->set_data(sb.GetString());
      chimera->clear_serialized();
    }
  }
}



void on_message(client* cli, websocketpp::connection_hdl hdl, message_ptr msg){
  Document d;
  StringBuffer sb;
  Writer<StringBuffer> w(sb);
  rapidjson::Document::AllocatorType &alloc = d.GetAllocator();

  Document ret;
  StringBuffer sb2;
  Writer<StringBuffer> w2(sb2);
  rapidjson::Document::AllocatorType &alloc2 = ret.GetAllocator();


  ParseResult ok = d.Parse(msg->get_payload().c_str(), msg->get_payload().size());
  if(!ok || !d.HasMember("type"))
  {
    return;
  }
  if(d["type"] == "telemetry_set_sesh_config"){
    if(d["data"].HasMember("pilot") &&
      d["data"].HasMember("circuit") &&
      d["data"].HasMember("configuration") &&
      d["data"].HasMember("test"))
    {
      sesh_config.configuration = d["data"]["configuration"].GetString();
      sesh_config.test = d["data"]["test"].GetString();
      if(d["data"]["pilot"].GetString() != "")
        sesh_config.pilot = d["data"]["pilot"].GetString();
      if(d["data"]["circuit"].GetString() != "")
        sesh_config.circuit = d["data"]["circuit"].GetString();

      save_all_config();
    }
    else{
      cout << "Wrong members" << endl;
    }
  }
  if(d["type"] == "telemetry_set_tel_config")
  {
    telemetry_config buffer;
    auto j = json::parse(msg->get_payload());
    try
    {
      Deserialize(buffer, j["data"]);
    }
    catch(const std::exception& e)
    {
      console->WarnMessage("Failed parsing telemetry config (from ws) " + msg->get_payload());
    }
    tel_conf = buffer;

    save_all_config();
  }
  else if(d["type"] == "ping")
  {
    console->DebugMessage("Requested ping");
    ret.SetObject();
    ret.AddMember("type", Value().SetString("server_answer_ping"), alloc2);
    ret.AddMember("time", (get_timestamp() - d["time"].GetDouble()), alloc2);
    ret.Accept(w2);
    
    ws_cli->set_data(sb2.GetString());
  } else if(d["type"] == "telemetry_get_config")
  {
    console->DebugMessage("Requested configs");

    string conf1 = Serialize(tel_conf).dump();
    string conf2 = Serialize(sesh_config).dump();

    ret.SetObject();
    ret.AddMember("type", Value().SetString("telemetry_config"), alloc2);
    ret.AddMember("telemetry_config", Value().SetString(conf1.c_str(), conf1.size(), alloc2), alloc2);
    ret.AddMember("session_config", Value().SetString(conf2.c_str(), conf2.size(), alloc2), alloc2);
    ret.Accept(w2);
    
    ws_cli->set_data(sb2.GetString());
    console->DebugMessage("Done config");
  } 
  else if(d["type"] == "telemetry_kill")
  {
    console->DebugMessage("Requested Kill");
    exit(0);
  }
  else if(d["type"] == "telemetry_start")
  {
    console->DebugMessage("Requested Start");
    ws_reqeust_on = true;
  }
  else if(d["type"] == "telemetry_stop")
  {
    console->DebugMessage("Requested Stop");
    ws_reqeust_off = true;
  }
  else if(d["type"] == "telemetry_send_can_message")
  {
    console->DebugMessage("Requested to send a can message to car");
    if((d.HasMember("id") && d.HasMember("payload")) &&
       (d["id"].IsInt() && d["payload"].IsArray()))
    {
      auto payload = d["payload"].GetArray();
      if(payload.Size() > 0 && payload.Size() <= 8 && payload[0].IsInt())
      {
        char msg[8];
        for(int i = 0 ; i < payload.Size(); i++)
          msg[i] = (char)payload[i].GetInt();
        can->send(d["id"].GetInt(), msg, 8);
      }else{
        console->DebugMessage("Can message malformed");
      }
    }
    else{
      console->DebugMessage("Can message malformed");
    }
  }
}


void connect_ws()
{
  ws_cli->add_on_open(on_open);
  ws_cli->add_on_error(on_error);
  ws_cli->add_on_close(on_close);

  // sends sensors data only if connected
  data_thread = new thread(send_ws_data);
  status_thread = new thread(send_status);
  while(true)
  {
    usleep(1000000);
    if(!tel_conf.ws_enabled)
      continue;
    if(ws_conn_state == ConnectionState_::CONNECTED || ws_conn_state == ConnectionState_::CONNECTING)
      continue;
    
    ws_cli->set_on_message(&on_message);
    ws_cli_thread = ws_cli->run(tel_conf.ws_server_url);
    if(ws_cli_thread == nullptr){
      console->ErrorMessage("Failed connecting to server: " + tel_conf.ws_server_url);
    }
    // Login as telemetry
    ws_cli->clear_data();
    ws_cli->set_data("{\"identifier\":\"telemetry\"}");
    ws_conn_state = ConnectionState_::CONNECTING;
  }
}

void on_open()
{
  ws_conn_state = ConnectionState_::CONNECTED;
}
void on_error(int code)
{
  ws_conn_state = ConnectionState_::FAIL;
}
void on_close(int code)
{
  ws_conn_state = ConnectionState_::CLOSED;
}