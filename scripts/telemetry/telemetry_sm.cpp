#include "telemetry_sm.h"

TelemetrySM::TelemetrySM()
: StateMachine(ST_MAX_STATES)
{
  HOME_PATH = getenv("HOME");
  CONSOLE.SaveAllMessages(HOME_PATH + "/telemetry_log.debug");

  currentError = TelemetryError::TEL_NONE;

	StatesStr[ST_UNINITIALIZED]   = "ST_UNINITIALIZED";
	StatesStr[ST_INIT]            = "ST_INIT";
	StatesStr[ST_IDLE]            = "ST_IDLE";
	StatesStr[ST_RUN]             = "ST_RUN";
	StatesStr[ST_STOP]            = "ST_STOP";
	StatesStr[ST_ERROR]           = "ST_ERROR";

  can = nullptr;
  dump_file = nullptr;
  chimera = nullptr;
  ws_cli = nullptr;
  data_thread = nullptr;
  status_thread = nullptr;
  ws_conn_thread = nullptr;
  ws_cli_thread = nullptr;

  kill_threads.store(false);
}

TelemetrySM::~TelemetrySM()
{
  EN_Deinitialize(nullptr);
}

void TelemetrySM::Init()
{
  BEGIN_TRANSITION_MAP
    TRANSITION_MAP_ENTRY(ST_INIT      ) // NONE
    TRANSITION_MAP_ENTRY(EVENT_IGNORED) // INIT
    TRANSITION_MAP_ENTRY(ST_INIT      ) // IDLE
    TRANSITION_MAP_ENTRY(EVENT_IGNORED) // RUN
    TRANSITION_MAP_ENTRY(ST_INIT      ) // STOP
    TRANSITION_MAP_ENTRY(ST_INIT      ) // ERROR
  END_TRANSITION_MAP(nullptr)
}

void TelemetrySM::Run()
{
  BEGIN_TRANSITION_MAP
    TRANSITION_MAP_ENTRY(EVENT_IGNORED) // NONE
    TRANSITION_MAP_ENTRY(EVENT_IGNORED) // INIT
    TRANSITION_MAP_ENTRY(ST_RUN       ) // IDLE
    TRANSITION_MAP_ENTRY(EVENT_IGNORED) // RUN
    TRANSITION_MAP_ENTRY(EVENT_IGNORED) // STOP
    TRANSITION_MAP_ENTRY(EVENT_IGNORED) // ERROR
  END_TRANSITION_MAP(nullptr)
}

void TelemetrySM::Stop()
{
  BEGIN_TRANSITION_MAP
    TRANSITION_MAP_ENTRY(EVENT_IGNORED) // NONE
    TRANSITION_MAP_ENTRY(EVENT_IGNORED) // INIT
    TRANSITION_MAP_ENTRY(EVENT_IGNORED) // IDLE
    TRANSITION_MAP_ENTRY(ST_STOP      ) // RUN
    TRANSITION_MAP_ENTRY(EVENT_IGNORED) // STOP
    TRANSITION_MAP_ENTRY(EVENT_IGNORED) // ERROR
  END_TRANSITION_MAP(nullptr)
}

void TelemetrySM::Reset()
{
  BEGIN_TRANSITION_MAP
    TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // NONE
    TRANSITION_MAP_ENTRY(ST_UNINITIALIZED)  // INIT
    TRANSITION_MAP_ENTRY(ST_UNINITIALIZED)  // IDLE
    TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // RUN
    TRANSITION_MAP_ENTRY(ST_UNINITIALIZED)  // STOP
    TRANSITION_MAP_ENTRY(ST_UNINITIALIZED)  // ERROR
  END_TRANSITION_MAP(nullptr)
}


STATE_DEFINE(TelemetrySM, UninitializedImpl, NoEventData)
{
  CONSOLE.LogStatus("UNINITIALIZED");
}

STATE_DEFINE(TelemetrySM, InitImpl, NoEventData)
{
  CONSOLE.LogStatus("INIT");
  // Loading json configurations
  CONSOLE.Log("Loading all config");
  LoadAllConfig();
  TEL_ERROR_CHECK
  CONSOLE.Log("Done");

  CONSOLE.Log("Opening log folders");
  FOLDER_PATH = HOME_PATH + "/logs";
  OpenLogFolder(FOLDER_PATH);
  TEL_ERROR_CHECK
  CONSOLE.Log("Done");


  // FOLDER_PATH/<date>/<session>
  sesh_config.Date = GetDate();
  FOLDER_PATH += "/" + sesh_config.Date;
  if(!path_exists(FOLDER_PATH))
    create_directory(FOLDER_PATH);


  CAN_DEVICE = tel_conf.can_device;
  CONSOLE.Log("Opening can socket");
  can = new Can();
  OpenCanSocket();
  TEL_ERROR_CHECK
  CONSOLE.Log("Done");


  chimera = new Chimera();
  ws_cli = new WebSocketClient();
  ws_conn_thread = new thread(&TelemetrySM::ConnectToWS, this);
  TEL_ERROR_CHECK

  SetupGps();
  TEL_ERROR_CHECK
  usleep(100000);
  CONSOLE.Log("Starting gps loggers");
  for(auto logger : gps_loggers)
    logger->Start();
  CONSOLE.Log("Done");

  if(tel_conf.camera_enable)
  {
#ifdef WITH_CAMERA
    CamInitData initData;
    initData.framerate = 24;
    initData.width = 320;
    initData.height = 240;
    CONSOLE.Log("Initializing Camera");
    camera.Init(&initData);
    CONSOLE.Log("Done");
#endif
  }

  InternalEvent(ST_IDLE);
  CONSOLE.LogStatus("INIT DONE");
}

STATE_DEFINE(TelemetrySM, IdleImpl, NoEventData)
{
  CONSOLE.LogStatus("IDLE");

  can_frame message;
  double timestamp;
  vector<Device *> modifiedDevices;
  while (GetCurrentState() == ST_IDLE)
  {
    can->receive(&message);
    timestamp = get_timestamp();
    msgs_counters["can"] ++;

    if(wsRequestState == ST_RUN || (message.can_id  == 0xA0 && message.can_dlc >= 2 &&
      message.data[0] == 0x66 && message.data[1] == 0x01))
    {
      wsRequestState = ST_UNINITIALIZED;
      InternalEvent(ST_RUN);
      break;
    }

    try{
      chimera->parse_message(timestamp, message.can_id, message.data, message.can_dlc, modifiedDevices);
    }
    catch(std::exception ex)
    {
      CONSOLE.LogError("Exception when parsing CAN message");
      CONSOLE.LogError("CAN message: ", CanMessage2Str(message));
      CONSOLE.LogError("Exception: ", ex.what());
      continue;
    }

    // For every device that has been modified by the parse operation
    for (auto modified : modifiedDevices)
    {
      unique_lock<mutex> lck(mtx);
      ProtoSerialize(timestamp, modified);
    }
  }
  CONSOLE.LogStatus("IDLE DONE");
}

ENTRY_DEFINE(TelemetrySM, ToRun, NoEventData)
{
  CONSOLE.LogStatus("TO_RUN");

  sesh_config.Time = GetTime();

  // Insert header at top of the file
  static string header;
  CreateHeader(header);
  
  static string folder;
  static string subfolder;
  CreateFolderName(subfolder);

  CONSOLE.Log("Creting new directory");
  // Adding incremental number at the end of foldername  
  int i = 1;
  do{
    folder = FOLDER_PATH + "/" + subfolder + " " + to_string(i);
    i++;
  }while(path_exists(folder));
  create_directory(folder);
  CURRENT_LOG_FOLDER = folder;
  CONSOLE.Log("Log folder: ", CURRENT_LOG_FOLDER);
  CONSOLE.Log("Done");

  CONSOLE.Log("Initializing loggers, and csv files");
  for(auto logger : gps_loggers)
  {
    logger->SetOutputFolder(CURRENT_LOG_FOLDER);
    logger->SetHeader(header);
  }
  CONSOLE.Log("Loggers DONE");

  dump_file = new std::fstream(CURRENT_LOG_FOLDER + "/" + "candump.log", std::fstream::out);
  (*dump_file) << header << "\n";

  if(tel_conf.generate_csv)
  {
    if(!path_exists(CURRENT_LOG_FOLDER + "/Parsed"))
      create_directory(CURRENT_LOG_FOLDER + "/Parsed");
      
    chimera->add_filenames(CURRENT_LOG_FOLDER + "/Parsed", ".csv");
    chimera->open_all_files();
    chimera->write_all_headers(0);
  }
  CONSOLE.Log("CSV Done");

  can_stat.msg_count = 0;
  can_stat.duration = get_timestamp();

  for(auto logger : gps_loggers)
    logger->StartLogging();

  if(tel_conf.camera_enable)
  {
#ifdef WITH_CAMERA
    CamRunData runData;
    runData.filename = CURRENT_LOG_FOLDER + "/" + "onboard.avi";
    CONSOLE.Log("Starting camera");
    camera.Run(&runData);
    CONSOLE.Log("Done");
#endif
  }
  CONSOLE.LogStatus("TO_RUN DONE");
}

STATE_DEFINE(TelemetrySM, RunImpl, NoEventData)
{
  CONSOLE.LogStatus("RUN");

  static can_frame message;
  double timestamp;
  vector<Device *> modifiedDevices;
  while(GetCurrentState() == ST_RUN)
  {
    timestamp = get_timestamp();
    can->receive(&message);
    can_stat.msg_count++;
    msgs_counters["can"] ++;


    LogCan(timestamp, message);


    // Parse the message only if is needed
    // Parsed messages are for sending via websocket or to be logged in csv
    if(tel_conf.generate_csv || tel_conf.ws_enabled)
    {
      try{
        chimera->parse_message(timestamp, message.can_id, message.data, message.can_dlc, modifiedDevices);
      }
      catch(std::exception ex)
      {
        CONSOLE.LogError("Exception when parsing CAN message");
        CONSOLE.LogError("CAN message: ", CanMessage2Str(message));
        CONSOLE.LogError("Exception: ", ex.what());
        continue;
      }

      // For every device that has been modified by the parse operation
      for (auto modified : modifiedDevices)
      {
        unique_lock<mutex> lck(mtx);

        if(tel_conf.generate_csv &&
          modified->files.size() > 0 && modified->files[0] != nullptr)
        {
          (*modified->files[0]) << modified->get_string(",") + "\n";
        }

        ProtoSerialize(timestamp, modified);
      }
    }

    // Stop message
    if (wsRequestState == ST_STOP || (message.can_id  == 0xA0 && message.can_dlc >= 2 &&
        message.data[0] == 0x66 && message.data[1] == 0x00))
    {
      wsRequestState = ST_UNINITIALIZED;
      InternalEvent(ST_STOP);
      break;
    }
  }
  CONSOLE.LogStatus("RUN DONE");
}

STATE_DEFINE(TelemetrySM, StopImpl, NoEventData)
{
  CONSOLE.LogStatus("STOP");

  unique_lock<mutex> lck(mtx);
  // duration of the log
  can_stat.duration = get_timestamp() - can_stat.duration;

  CONSOLE.Log("Closing files");
  if(tel_conf.generate_csv){
    // Close all csv files and the dump file
    chimera->close_all_files();
  }
  dump_file->close();
  delete dump_file;
  CONSOLE.Log("Done");

  CONSOLE.Log("Restarting gps loggers");
  // Stop logging but continue reading port
  for(auto logger : gps_loggers)
  {
    logger->StopLogging();
    logger->Start();
  }
  CONSOLE.Log("Done");


  if(tel_conf.camera_enable)
  {
#ifdef WITH_CAMERA
    CONSOLE.Log("Stopping Camera");
    camera.Stop();
    CONSOLE.Log("Done");
#endif
  }


  // Save stats of this log session
  CONSOLE.Log("Saving stat: ", CURRENT_LOG_FOLDER);
  SaveStat();
  CONSOLE.Log("Done");

  InternalEvent(ST_IDLE);
  CONSOLE.LogStatus("STOP DONE");
}

STATE_DEFINE(TelemetrySM, ErrorImpl, NoEventData)
{
  CONSOLE.LogError("Error occurred");
  CONSOLE.LogStatus("ERROR");
}

ENTRY_DEFINE(TelemetrySM, Deinitialize, NoEventData)
{
  CONSOLE.LogStatus("DEINITIALIZE");

  kill_threads.store(true);

  if(dump_file != nullptr)
  {
    if(dump_file->is_open())
      dump_file->close();
    delete dump_file;
    dump_file = nullptr;
  }
  CONSOLE.Log("Closed dump file");

  if(can != nullptr && can->is_open())
  {
    can->close_socket();
    delete can;
    can = nullptr;
  }
  CONSOLE.Log("Closed can socket");

  if(chimera != nullptr)
  {
    chimera->clear_serialized();
    chimera->close_all_files();
    delete chimera;
    chimera = nullptr;
  }
  CONSOLE.Log("Deleted vehicle");

  for(int i = 0; i < gps_loggers.size(); i++)
  {
    if(gps_loggers[i] == nullptr)
      continue;
    gps_loggers[i]->Kill();
    gps_loggers[i]->WaitForEnd();
    delete gps_loggers[i];
  }
  gps_loggers.resize(0);
  CONSOLE.Log("Closed gps loggers");

  if(data_thread != nullptr)
  {
    if(data_thread->joinable())
      data_thread->join();
    delete data_thread;
    data_thread = nullptr;
  }
  CONSOLE.Log("Stopped data thread");
  if(status_thread != nullptr)
  {
    if(status_thread->joinable())
      status_thread->join();
    delete status_thread;
    status_thread = nullptr;
  }
  CONSOLE.Log("Stopped status thread");
  if(ws_conn_thread != nullptr)
  {
    if(ws_conn_thread->joinable())
      ws_conn_thread->join();
    delete ws_conn_thread;
    ws_conn_thread = nullptr;
  }
  CONSOLE.Log("Stopped reconnection thread");

  if(ws_cli != nullptr)
  {
    ws_cli->clear_data();
    ws_cli->close();
    if(ws_cli_thread != nullptr)
    {
      if(ws_cli_thread->joinable())
        ws_cli_thread->join();
      delete ws_cli_thread;
      ws_cli_thread = nullptr;
    }
    delete ws_cli;
    ws_cli = nullptr;
  }
  CONSOLE.Log("Stopped connection");

  msgs_counters.clear();
  msgs_per_second.clear();
  timers.clear();

  currentError = TelemetryError::TEL_NONE;

  kill_threads.store(false);

  CONSOLE.Log("Cleared variables");

  CONSOLE.LogStatus("DEINITIALIZE DONE");
}


TelemetryError TelemetrySM::GetError()
{
  return currentError;
}
void TelemetrySM::EmitError(TelemetryError error)
{
  currentError = error;
  CONSOLE.LogError(TelemetryErrorStr[error]);

  InternalEvent(ST_ERROR);
}

// Load all configuration files
// If the file doesn't exist create it
void TelemetrySM::LoadAllConfig()
{
  string path = HOME_PATH + "/telemetry_config.json";
  if(path_exists(path))
  {
    if(LoadJson(tel_conf, path))
      CONSOLE.Log("Loaded telemetry config");
    else
      EmitError(TEL_LOAD_CONFIG_TELEMETRY);
  }else{
    CONSOLE.Log("Created: " + path);
    tel_conf.can_device = "can0";
    tel_conf.generate_csv = false;
    tel_conf.ws_enabled = true;
    tel_conf.ws_send_sensor_data = true;
    tel_conf.ws_send_rate = 500;
    tel_conf.ws_downsample = true;
    tel_conf.ws_downsample_mps = 50;
    tel_conf.ws_server_url = "ws://eagle-telemetry-server.herokuapp.com";
    SaveJson(tel_conf, path);
  }

  path = HOME_PATH + "/session_config.json";
  if(path_exists(path))
  {
    if(LoadJson(sesh_config, path))
      CONSOLE.Log("Loaded session config");
    else
      EmitError(TEL_LOAD_CONFIG_SESSION);
  }else{
    CONSOLE.Log("Created: " + path);
    SaveJson(sesh_config, path);
  }
}
void TelemetrySM::SaveAllConfig()
{
  string path = " ";
  path = HOME_PATH + "/telemetry_config.json";
  CONSOLE.Log("Saving new tel config");
  SaveJson(tel_conf, path);
  CONSOLE.Log("Done");
  CONSOLE.Log("Saving new sesh config");
  path = HOME_PATH + "/session_config.json";
  SaveJson(sesh_config, path);
  CONSOLE.Log("Done");
}

void TelemetrySM::SaveStat()
{
  Document doc;
  StringBuffer json_ss;
  PrettyWriter<StringBuffer> writer(json_ss);
  rapidjson::Document::AllocatorType &alloc = doc.GetAllocator();

  doc.SetObject();
  // Add keys and string values
  doc.AddMember("Date", Value().SetString(StringRef(sesh_config.Date.c_str())), alloc);
  doc.AddMember("Time", Value().SetString(StringRef(sesh_config.Time.c_str())), alloc);
  doc.AddMember("Circuit", Value().SetString(StringRef(sesh_config.Circuit.c_str())), alloc);
  doc.AddMember("Pilot", Value().SetString(StringRef(sesh_config.Pilot.c_str())), alloc);
  doc.AddMember("Race", Value().SetString(StringRef(sesh_config.Race.c_str())), alloc);
  doc.AddMember("Configuration", Value().SetString(StringRef(sesh_config.Configuration.c_str())), alloc);

  Value val;
  val.SetObject();
  {
    val.AddMember("Messages", can_stat.msg_count, alloc);
    val.AddMember("Average Frequency (Hz)", int(double(can_stat.msg_count) / can_stat.duration), alloc);
    val.AddMember("Duration (seconds)", can_stat.duration, alloc);
  }
  doc.AddMember("CAN", val, alloc);

  doc.Accept(writer);
  std::ofstream stat_f(CURRENT_LOG_FOLDER + "/CAN_Info.json");
  stat_f << json_ss.GetString();
  stat_f.close();
}

void TelemetrySM::CreateHeader(string& out)
{
  out = "\r\n\n";
  out += "*** EAGLE-TRT\r\n";
  out += "*** Telemetry Log File\r\n";
  out += "*** Date: " + sesh_config.Date + "\r\n";
  out += "*** Time: " + sesh_config.Time + "\r\n";
  out += "\r\n";
  out += "*** Curcuit       .... " + sesh_config.Circuit + "\r\n";
  out += "*** Pilot         .... " + sesh_config.Pilot + "\r\n";
  out += "*** Race          .... " + sesh_config.Race + "\r\n";
  out += "*** Configuration .... " + sesh_config.Configuration;
  out += "\n\n\r";
}
void TelemetrySM::CreateFolderName(string& out)
{ 
  // Create a folder with current configurations
  stringstream subfolder;
  subfolder << sesh_config.Race;
  subfolder << " [";
  subfolder << sesh_config.Configuration << "]";

  string s = subfolder.str();
  std::replace(s.begin(), s.end(), '\\', ' ');
  std::replace(s.begin(), s.end(), '/', ' ');
  std::replace(s.begin(), s.end(), '\n', ' ');

  out = s;
}
void TelemetrySM::LogCan(const double& timestamp, const can_frame& msg)
{
  string line = "";
  line += "(" + to_string(timestamp) + ")\t" + CAN_DEVICE + "\t";
  line += CanMessage2Str(msg);
  line += "\n";

  (*dump_file) << line;
}

string TelemetrySM::GetDate()
{
  static time_t date;
  static struct tm ltm;
  time(&date);
  localtime_r(&date, &ltm);
  std::ostringstream ss;
  ss << std::put_time(&ltm, "%d_%m_%Y");
  return ss.str();
}
string TelemetrySM::GetTime()
{
  static time_t date;
  static struct tm ltm;
  time(&date);
  localtime_r(&date, &ltm);
  std::ostringstream ss;
  ss << std::put_time(&ltm, "%H:%M:%S");
  return ss.str();
}
string TelemetrySM::CanMessage2Str(const can_frame& msg)
{
  string out = "";
  // Format message as ID#<payload>
  // Hexadecimal representation
  out += get_hex(int(msg.can_id), 3) + "#";
  for (int i = 0; i < msg.can_dlc; i++)
  {
    out += get_hex(int(msg.data[i]), 2);
  }
  return out;
}


void TelemetrySM::OpenLogFolder(const string& path)
{
  CONSOLE.Log("Output Folder ", path);
  if (!path_exists(path))
  {
    CONSOLE.LogWarn("Creating log folder");
    if(!create_directory(path))
      EmitError(TEL_LOG_FOLDER);
  }
}

void TelemetrySM::OpenCanSocket()
{
  can->init(CAN_DEVICE.c_str(), &addr);

  if (can->open_socket() < 0)
  {
    CONSOLE.LogWarn("Failed binding socket: ", CAN_DEVICE);
    CAN_DEVICE = "vcan0";
    can->init(CAN_DEVICE.c_str(), &addr);
    if (can->open_socket() < 0)
    {
      CONSOLE.LogWarn("Failed binding socket: ", CAN_DEVICE);
      EmitError(TEL_CAN_SOCKET_OPEN);
      return;
    }
  }
  CONSOLE.Log("Opened Socket: ", CAN_DEVICE);
}


void TelemetrySM::SetupGps()
{
  CONSOLE.Log("Initializing gps instances");
  // Setup of all GPS devices
  for(size_t i = 0; i < tel_conf.gps_devices.size(); i++)
  {
    string dev = tel_conf.gps_devices[i];
    string mode = tel_conf.gps_mode[i];
    bool enabled = tel_conf.gps_enabled[i];
    if(dev == "")
      continue;

    CONSOLE.Log("Initializing ", dev, mode, enabled);

    GpsLogger* gps = new GpsLogger(dev);
    gps->SetOutFName("gps_" + to_string(i));
    msgs_counters["gps_" + to_string(i)] = 0;
    msgs_per_second["gps_" + to_string(i)] = 0;
    if(mode == "file")
      gps->SetMode(MODE_FILE);
    else
      gps->SetMode(MODE_PORT);
    gps->SetCallback(bind(&TelemetrySM::OnGpsLine, this, std::placeholders::_1, std::placeholders::_2));

    gps_loggers.push_back(gps);
  }
  CONSOLE.Log("Done");
}
// Callback, fires every time a line from a GPS is received
void TelemetrySM::OnGpsLine(int id, string line)
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
    CONSOLE.LogError("GPS parse error: ", line);
    return;
  }

  // If parsing was successfull
  // save parsed data into gps file
  if(ret == 1)
  {
    msgs_counters["gps_" + to_string(id)] ++;

    unique_lock<mutex> lck(mtx);
      
    if(GetCurrentState() == ST_RUN && tel_conf.generate_csv)
    {
      (*gps->files[0]) << gps->get_string(",") + "\n" << flush;
    }
    if(tel_conf.ws_send_sensor_data)
      chimera->serialize_device(gps);
  }
  else
  {
    // cout << ret << " " << line << endl;
  }
}


void TelemetrySM::ConnectToWS()
{
  ws_cli->add_on_open(bind(&TelemetrySM::OnOpen, this));
  ws_cli->add_on_close(bind(&TelemetrySM::OnClose, this, std::placeholders::_1));
  ws_cli->add_on_error(bind(&TelemetrySM::OnError, this, std::placeholders::_1));

  // sends sensors data only if connected
  data_thread = new thread(&TelemetrySM::SendWsData, this);
  status_thread = new thread(&TelemetrySM::SendStatus, this);
  while(kill_threads.load() == false)
  {
    usleep(1000000);
    if(!tel_conf.ws_enabled)
      continue;
    if(ws_conn_state == ConnectionState_::CONNECTED || ws_conn_state == ConnectionState_::CONNECTING)
      continue;
    
    ws_cli->set_on_message(bind(&TelemetrySM::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    ws_cli_thread = ws_cli->run(tel_conf.ws_server_url);
    if(ws_cli_thread == nullptr){
      CONSOLE.ErrorMessage("Failed connecting to server: " + tel_conf.ws_server_url);
    }
    // Login as telemetry
    ws_cli->clear_data();
    ws_cli->set_data("{\"identifier\":\"telemetry\"}");
    ws_conn_state = ConnectionState_::CONNECTING;
  }
}

void TelemetrySM::OnMessage(client* cli, websocketpp::connection_hdl hdl, message_ptr msg)
{
  Document req;
  StringBuffer sb;
  Writer<StringBuffer> w(sb);
  rapidjson::Document::AllocatorType &alloc = req.GetAllocator();

  Document ret;
  StringBuffer sb2;
  Writer<StringBuffer> w2(sb2);
  rapidjson::Document::AllocatorType &alloc2 = ret.GetAllocator();


  ParseResult ok = req.Parse(msg->get_payload().c_str(), msg->get_payload().size());
  if(!ok || !req.HasMember("type"))
  {
    return;
  }
  if(req["type"] == "telemetry_set_sesh_config"){
    if(req["data"].HasMember("Pilot") &&
      req["data"].HasMember("Circuit") &&
      req["data"].HasMember("Configuration") &&
      req["data"].HasMember("Race"))
    {
      sesh_config.Configuration = req["data"]["Configuration"].GetString();
      sesh_config.Race = req["data"]["Race"].GetString();
      if(req["data"]["Pilot"].GetString() != "")
        sesh_config.Pilot = req["data"]["Pilot"].GetString();
      if(req["data"]["Circuit"].GetString() != "")
        sesh_config.Circuit = req["data"]["Circuit"].GetString();

      SaveAllConfig();
    }
    else{
      CONSOLE.LogWarn("Telemetry set session config (Wrong members)");
    }
  }
  if(req["type"] == "telemetry_set_tel_config")
  {
    telemetry_config buffer;
    auto j = json::parse(msg->get_payload());
    try
    {
      Deserialize(buffer, j["data"]);
    }
    catch(const std::exception& e)
    {
      CONSOLE.LogWarn("Failed parsing telemetry config (from ws) ", msg->get_payload());
    }
    tel_conf = buffer;

    SaveAllConfig();
  }
  else if(req["type"] == "ping")
  {
    CONSOLE.DebugMessage("Requested ping");
    ret.SetObject();
    ret.AddMember("type", Value().SetString("server_answer_ping"), alloc2);
    ret.AddMember("time", (get_timestamp() - req["time"].GetDouble()), alloc2);
    ret.Accept(w2);
    
    ws_cli->set_data(sb2.GetString());
  } else if(req["type"] == "telemetry_get_config")
  {
    CONSOLE.DebugMessage("Requested configs");

    string conf1 = Serialize(tel_conf).dump();
    string conf2 = Serialize(sesh_config).dump();

    ret.SetObject();
    ret.AddMember("type", Value().SetString("telemetry_config"), alloc2);
    ret.AddMember("telemetry_config", Value().SetString(conf1.c_str(), conf1.size(), alloc2), alloc2);
    ret.AddMember("session_config", Value().SetString(conf2.c_str(), conf2.size(), alloc2), alloc2);
    ret.Accept(w2);
    
    ws_cli->set_data(sb2.GetString());
    CONSOLE.Log("Done config");
  } 
  else if(req["type"] == "telemetry_kill")
  {
    CONSOLE.Log("Requested Kill (from ws)");
    exit(0);
  }
  else if(req["type"] == "telemetry_start")
  {
    CONSOLE.Log("Requested Start (from ws)");
    wsRequestState = ST_RUN;
  }
  else if(req["type"] == "telemetry_stop")
  {
    CONSOLE.Log("Requested Stop (from ws)");
    wsRequestState = ST_STOP;
  }
  else if(req["type"] == "telemetry_send_can_message")
  {
    CONSOLE.Log("Requested to send a can message to car");
    if((req.HasMember("id") && req.HasMember("payload")) &&
       (req["id"].IsInt() && req["payload"].IsArray()))
    {
      auto payload = req["payload"].GetArray();
      if(payload.Size() > 0 && payload.Size() <= 8 && payload[0].IsInt())
      {
        char msg[8];
        for(int i = 0 ; i < payload.Size(); i++)
          msg[i] = (char)payload[i].GetInt();
        can->send(req["id"].GetInt(), msg, 8);
      }else{
        CONSOLE.LogWarn("CAN message malformed (from ws)");
      }
    }
    else{
      CONSOLE.LogWarn("CAN message malformed (from ws)");
    }
  }
}

void TelemetrySM::SendStatus()
{
  char msg_data[8];
  int is_in_run;
  while(kill_threads.load() == false)
  {
    if(GetCurrentState() == ST_RUN)
      is_in_run = 1;
    else
      is_in_run = 0;
    msg_data[0] = is_in_run;
    can->send(0x99, msg_data, 1);

    
    msgs_per_second["can"] = msgs_counters["can"];
    for(int i = 0; i < gps_loggers.size(); i++)
    {
      msgs_per_second["gps_"+to_string(i)] = msgs_counters["gps_"+to_string(i)];
      msgs_counters["gps_"+to_string(i)] = 0;
    }
    msgs_counters["can"] = 0;
    CONSOLE.Log("Status",StatesStr[GetCurrentState()],"MSGS per second: CAN", msgs_per_second["can"],
            "gps_0", msgs_per_second["gps_0"], "gps_1", msgs_per_second["gps_1"]);

    if(tel_conf.ws_enabled && ws_conn_state == ConnectionState_::CONNECTED)
    {
      Document d;
      StringBuffer sb;
      Writer<StringBuffer> w(sb);
      rapidjson::Document::AllocatorType &alloc = d.GetAllocator();

      d.SetObject();
      d.AddMember("type", Value().SetString("telemetry_status"), alloc);
      d.AddMember("timestamp", get_timestamp(), alloc);
      d.AddMember("data", is_in_run, alloc);
      Value val;
      val.SetObject();
      for(auto el : msgs_per_second)
      {
        val.AddMember(Value().SetString(el.first.c_str(), alloc), el.second, alloc);
      }
      d.AddMember("msgs_per_second", val, alloc);
      auto cam_state = camera.StatesStr[camera.GetCurrentState()];
      auto cam_error = CamErrorStr[camera.GetError()];
      d.AddMember("camera_status", Value().SetString(cam_state.c_str(), cam_state.size(), alloc), alloc);
      d.AddMember("camera_error", Value().SetString(cam_error.c_str(), cam_error.size(), alloc), alloc);

      d.Accept(w);

      ws_cli->set_data(sb.GetString());
    }


    usleep(1000000);
  }
}

void TelemetrySM::SendWsData()
{
  Document d;
  StringBuffer sb;
  Writer<StringBuffer> w(sb);
  rapidjson::Document::AllocatorType &alloc = d.GetAllocator();
  while(kill_threads.load() == false)
  {
    while(ws_conn_state != ConnectionState_::CONNECTED)
      usleep(500000);
    while(ws_conn_state == ConnectionState_::CONNECTED)
    {
      usleep(1000 * tel_conf.ws_send_rate);

      if(!tel_conf.ws_send_sensor_data)
        continue;

      unique_lock<mutex> lck(mtx);

      string serialized_string;
      chimera->serialized_to_string(&serialized_string);

      if(serialized_string.size() == 0)
      {
        continue;
      }

      sb.Clear();
      w.Reset(sb);
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

void TelemetrySM::ProtoSerialize(const double& timestamp, Device* device)
{
  // Serialize with protobuf if websocket is enabled
  if(tel_conf.ws_enabled && tel_conf.ws_send_sensor_data)
  {
    if(tel_conf.ws_downsample == true)
    {
      if((1.0/tel_conf.ws_downsample_mps) < (timestamp - timers[device->get_name()]))
      {
        timers[device->get_name()] = timestamp;
        chimera->serialize_device(device);
      }
    }else
    {
      chimera->serialize_device(device);
    }
  }
}

void TelemetrySM::OnOpen()
{
  CONSOLE.Log("WS opened");
  ws_conn_state = ConnectionState_::CONNECTED;
}
void TelemetrySM::OnClose(int code)
{
  CONSOLE.LogError("WS Closed");
  ws_conn_state = ConnectionState_::CLOSED;
}
void TelemetrySM::OnError(int code)
{
  CONSOLE.LogError("WS Error");
  ws_conn_state = ConnectionState_::FAIL;
}