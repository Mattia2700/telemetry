#include "telemetry_sm.h"

TelemetrySM::TelemetrySM()
    : StateMachine(ST_MAX_STATES)
{
  if (getenv("HOME") != NULL)
    HOME_PATH = getenv("HOME");
  else
    HOME_PATH = "/home/filippo";
  CONSOLE.SaveAllMessages(HOME_PATH + "/telemetry_log.debug");

  currentError = TelemetryError::TEL_NONE;

  StatesStr[ST_UNINITIALIZED] = "ST_UNINITIALIZED";
  StatesStr[ST_INIT] = "ST_INIT";
  StatesStr[ST_IDLE] = "ST_IDLE";
  StatesStr[ST_RUN] = "ST_RUN";
  StatesStr[ST_STOP] = "ST_STOP";
  StatesStr[ST_ERROR] = "ST_ERROR";

  dump_file = NULL;
  ws_cli = nullptr;
  data_thread = nullptr;
  status_thread = nullptr;
  ws_conn_thread = nullptr;
  ws_cli_thread = nullptr;
  actions_thread = nullptr;

  lp = nullptr;
  lp_inclination = nullptr;

  kill_threads.store(false);
  wsRequestState = ST_MAX_STATES;

  primary_devices_new(&primary_devs);
  secondary_devices_new(&secondary_devs);
  for (int i = 0; i < primary_NUMBER_OF_MESSAGES; i++)
    primary_files[i] = NULL;
  for (int i = 0; i < secondary_NUMBER_OF_MESSAGES; i++)
    secondary_files[i] = NULL;
}

TelemetrySM::~TelemetrySM()
{
  EN_Deinitialize(nullptr);
}

void TelemetrySM::Init()
{
  BEGIN_TRANSITION_MAP
  TRANSITION_MAP_ENTRY(ST_INIT)       // NONE
  TRANSITION_MAP_ENTRY(EVENT_IGNORED) // INIT
  TRANSITION_MAP_ENTRY(ST_INIT)       // IDLE
  TRANSITION_MAP_ENTRY(EVENT_IGNORED) // RUN
  TRANSITION_MAP_ENTRY(ST_INIT)       // STOP
  TRANSITION_MAP_ENTRY(ST_INIT)       // ERROR
  END_TRANSITION_MAP(nullptr)
}

void TelemetrySM::Run()
{
  BEGIN_TRANSITION_MAP
  TRANSITION_MAP_ENTRY(EVENT_IGNORED) // NONE
  TRANSITION_MAP_ENTRY(EVENT_IGNORED) // INIT
  TRANSITION_MAP_ENTRY(ST_RUN)        // IDLE
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
  TRANSITION_MAP_ENTRY(ST_STOP)       // RUN
  TRANSITION_MAP_ENTRY(EVENT_IGNORED) // STOP
  TRANSITION_MAP_ENTRY(EVENT_IGNORED) // ERROR
  END_TRANSITION_MAP(nullptr)
}

void TelemetrySM::Reset(){
    BEGIN_TRANSITION_MAP
        TRANSITION_MAP_ENTRY(EVENT_IGNORED) // NONE
    TRANSITION_MAP_ENTRY(ST_UNINITIALIZED)  // INIT
    TRANSITION_MAP_ENTRY(ST_UNINITIALIZED)  // IDLE
    TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // RUN
    TRANSITION_MAP_ENTRY(ST_UNINITIALIZED)  // STOP
    TRANSITION_MAP_ENTRY(ST_UNINITIALIZED)  // ERROR
    END_TRANSITION_MAP(nullptr)}

STATE_DEFINE(TelemetrySM, UninitializedImpl, NoEventData)
{
  CONSOLE.LogStatus("UNINITIALIZED");
}

STATE_DEFINE(TelemetrySM, InitImpl, NoEventData)
{
  CONSOLE.LogStatus("INIT");

  cpu_total_load_init();
  cpu_process_load_init();

  kill_threads.store(false);
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
  if (!path_exists(FOLDER_PATH))
    create_directory(FOLDER_PATH);

  CONSOLE.Log("Opening can socket");
  OpenCanSocket();
  TEL_ERROR_CHECK
  CONSOLE.Log("Done");

  CONSOLE.Log("Chimera and WS instances");
  // chimera = new Chimera();
  ws_cli = new WebSocketClient();
  ws_conn_thread = new thread(&TelemetrySM::ConnectToWS, this);
  actions_thread = new thread(&TelemetrySM::ActionThread, this);
  CONSOLE.Log("DONE");
  TEL_ERROR_CHECK

  SetupGps();
  TEL_ERROR_CHECK

  if (tel_conf.camera_enable)
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

  CONSOLE.Log("Starting gps loggers");
  for (size_t i = 0; i < gps_loggers.size(); i++)
    if (tel_conf.gps_enabled[i])
      gps_loggers[i]->Start();
  CONSOLE.Log("Done");

  CAN_Message message_q;
  can_frame message;
  uint64_t timestamp;
  vector<Device *> modifiedDevices;

  static FILE *fout;
  static int dev_idx;
  while (GetCurrentState() == ST_IDLE)
  {
    {
      unique_lock<mutex> lck(can_mutex);
      while (messages_queue.size() == 0)
        can_cv.wait(lck);
      message_q = messages_queue.back();
      messages_queue.pop();
    }
    timestamp = message_q.timestamp;
    can_stat.Messages++;
    msgs_counters[message_q.receiver_name]++;
    message = message_q.frame;

    if (message_q.receiver_name == "primary" && primary_is_message_id(message.can_id))
    {
      dev_idx = primary_devices_index_from_id(message.can_id, &primary_devs);
      primary_deserialize_from_id(message.can_id, message.data, primary_devs[dev_idx].raw_message, primary_devs[dev_idx].conversion_message, timestamp);
      ProtoSerialize(0, timestamp, message, dev_idx);

      if (message.can_id == primary_id_SET_TLM_STATUS)
      {
        primary_message_SET_TLM_STATUS *msg = ((primary_message_SET_TLM_STATUS *)primary_devs[dev_idx].raw_message);
        if (msg->tlm_status == primary_Toggle_ON)
          InternalEvent(ST_RUN);
      }
    }
    if (message_q.receiver_name == "secondary" && secondary_is_message_id(message.can_id))
    {
      dev_idx = secondary_devices_index_from_id(message.can_id, &secondary_devs);
      secondary_deserialize_from_id(message.can_id, message.data, secondary_devs[dev_idx].raw_message, secondary_devs[dev_idx].conversion_message, timestamp);
      ProtoSerialize(1, timestamp, message, dev_idx);
    }

    if (wsRequestState == ST_UNINITIALIZED)
    {
      wsRequestState = ST_MAX_STATES;
      InternalEvent(ST_UNINITIALIZED);
      break;
    }

    if (wsRequestState == ST_RUN)
    {
      wsRequestState = ST_MAX_STATES;
      InternalEvent(ST_RUN);
      break;
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
  do
  {
    folder = FOLDER_PATH + "/" + subfolder + " " + to_string(i);
    i++;
  } while (path_exists(folder));
  create_directory(folder);

  CURRENT_LOG_FOLDER = folder;
  CONSOLE.Log("Log folder: ", CURRENT_LOG_FOLDER);
  CONSOLE.Log("Done");

  CONSOLE.Log("Initializing loggers, and csv files");
  for (auto logger : gps_loggers)
  {
    if (!tel_conf.gps_enabled[i])
      continue;
    logger->SetOutputFolder(CURRENT_LOG_FOLDER);
    logger->SetHeader(header);
  }
  CONSOLE.Log("Loggers DONE");

  // dump_file = fopen((CURRENT_LOG_FOLDER + "/" + "candump.log").c_str(), "w");
  // if (dump_file == NULL)
  // {
  //   CONSOLE.LogError("Error opening candump file!");
  //   EmitError(TEL_LOG_FOLDER);
  // }
  // fprintf(dump_file, "%s\n", header.c_str());
  dump_file = new fstream((CURRENT_LOG_FOLDER + "/" + "candump.log").c_str(), std::fstream::out);
  if (!dump_file->is_open())
  {
    CONSOLE.LogError("Error opening candump file!");
    EmitError(TEL_LOG_FOLDER);
  }
  (*dump_file) << header << "\n";

  ///////
  if (tel_conf.generate_csv)
  {
    if (!path_exists(CURRENT_LOG_FOLDER + "/Parsed"))
      create_directory(CURRENT_LOG_FOLDER + "/Parsed");
    if (!path_exists(CURRENT_LOG_FOLDER + "/Parsed/primary"))
      create_directory(CURRENT_LOG_FOLDER + "/Parsed/primary");
    if (!path_exists(CURRENT_LOG_FOLDER + "/Parsed/secondary"))
      create_directory(CURRENT_LOG_FOLDER + "/Parsed/secondary");

    char buff[125];
    for (int i = 0; i < primary_NUMBER_OF_MESSAGES; i++)
    {
      primary_message_name_from_id(primary_devs[i].id, buff);
      string folder = (CURRENT_LOG_FOLDER + "/Parsed/primary/" + string(buff) + ".csv");
      primary_files[i] = fopen(folder.c_str(), "w");
      primary_fields_from_id(primary_devs[i].id, primary_files[i]);
      fprintf(primary_files[i], "\r\n");
    }
    for (int i = 0; i < secondary_NUMBER_OF_MESSAGES; i++)
    {
      secondary_message_name_from_id(secondary_devs[i].id, buff);
      string folder = (CURRENT_LOG_FOLDER + "/Parsed/secondary/" + string(buff) + ".csv");
      secondary_files[i] = fopen(folder.c_str(), "w");
      secondary_fields_from_id(secondary_devs[i].id, secondary_files[i]);
      fprintf(secondary_files[i], "\r\n");
    }
  }
  CONSOLE.Log("CSV Done");

  can_stat.Messages = 0;
  can_stat.Duration_seconds = get_timestamp();

  for (auto logger : gps_loggers)
    logger->StartLogging();

  if (tel_conf.camera_enable)
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

  can_frame message;
  CAN_Message message_q;
  uint64_t timestamp;

  static FILE *csv_out;
  static int dev_idx = 0;

  while (GetCurrentState() == ST_RUN)
  {
    {
      unique_lock<mutex> lck(can_mutex);
      while (messages_queue.size() == 0)
        can_cv.wait(lck);
      message_q = messages_queue.back();
      messages_queue.pop();
    }
    timestamp = message_q.timestamp;
    can_stat.Messages++;
    msgs_counters[message_q.receiver_name]++;
    message = message_q.frame;

    LogCan(message_q);

    // Parse the message only if is needed
    // Parsed messages are for sending via websocket or to be logged in csv
    if (tel_conf.generate_csv || tel_conf.ws_enabled)
    {
      if (message_q.receiver_name == "primary" && primary_is_message_id(message.can_id))
      {
        dev_idx = primary_devices_index_from_id(message.can_id, &primary_devs);
        primary_deserialize_from_id(message.can_id, message.data, primary_devs[dev_idx].raw_message, primary_devs[dev_idx].conversion_message, timestamp);
        if (tel_conf.generate_csv)
        {
          csv_out = primary_files[dev_idx];
          if (primary_devs[dev_idx].conversion_message == NULL)
            primary_string_from_id(message.can_id, primary_devs[dev_idx].raw_message, csv_out);
          else
            primary_string_from_id(message.can_id, primary_devs[dev_idx].conversion_message, csv_out);

          fprintf(csv_out, "\n");
        }
        ProtoSerialize(0, timestamp, message, dev_idx);
        if (message.can_id == primary_id_SET_TLM_STATUS)
        {
          primary_message_SET_TLM_STATUS *msg = ((primary_message_SET_TLM_STATUS *)primary_devs[dev_idx].raw_message);
          if (msg->tlm_status == primary_Toggle_OFF)
            InternalEvent(ST_STOP);
        }
      }
      if (message_q.receiver_name == "secondary" && secondary_is_message_id(message.can_id))
      {
        dev_idx = secondary_devices_index_from_id(message.can_id, &secondary_devs);
        secondary_deserialize_from_id(message.can_id, message.data, secondary_devs[dev_idx].raw_message, secondary_devs[dev_idx].conversion_message, timestamp);
        if (tel_conf.generate_csv)
        {
          csv_out = secondary_files[dev_idx];
          if (secondary_devs[dev_idx].conversion_message == NULL)
            secondary_string_from_id(message.can_id, secondary_devs[dev_idx].raw_message, csv_out);
          else
            secondary_string_from_id(message.can_id, secondary_devs[dev_idx].conversion_message, csv_out);
          fprintf(csv_out, "\n");
        }
        ProtoSerialize(1, timestamp, message, dev_idx);
      }
    }

    // Stop message
    if (wsRequestState == ST_STOP)
    {
      wsRequestState = ST_MAX_STATES;
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
  can_stat.Duration_seconds = get_timestamp() - can_stat.Duration_seconds;

  CONSOLE.Log("Closing files");
  if (tel_conf.generate_csv)
  {
    // Close all csv files and the dump file
    // chimera->close_all_files();
    for (int i = 0; i < primary_NUMBER_OF_MESSAGES; i++)
    {
      if (primary_files[i] != NULL)
      {
        fclose(primary_files[i]);
        primary_files[i] = NULL;
      }
    }
    for (int i = 0; i < secondary_NUMBER_OF_MESSAGES; i++)
    {
      if (secondary_files[i] != NULL)
      {
        fclose(secondary_files[i]);
        secondary_files[i] = NULL;
      }
    }
  }
  dump_file->close();
  dump_file = NULL;
  CONSOLE.Log("Done");

  CONSOLE.Log("Restarting gps loggers");
  // Stop logging but continue reading port
  for (int i = 0; i < gps_loggers.size(); i++)
  {
    gps_loggers[i]->StopLogging();
    if (tel_conf.gps_enabled[i])
      gps_loggers[i]->Start();
  }
  CONSOLE.Log("Done");

  if (tel_conf.camera_enable)
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

  /////////////////////////
  // LAP COUNTER DESTROY //
  /////////////////////////
  if (lp != nullptr)
  {
    lc_reset(lp); // reset object (removes everything but thresholds)
    lc_destroy(lp);
  }
  if (lp_inclination != nullptr)
  {
    lc_reset(lp_inclination);
    lc_destroy(lp_inclination);
  }

  kill_threads.store(true);

  if (actions_thread != nullptr)
  {
    if (actions_thread->joinable())
      actions_thread->join();
    delete actions_thread;
    actions_thread = nullptr;
  }
  CONSOLE.Log("Stopped actions thread");

  if (data_thread != nullptr)
  {
    if (data_thread->joinable())
      data_thread->join();
    delete data_thread;
    data_thread = nullptr;
  }
  CONSOLE.Log("Stopped data thread");
  if (status_thread != nullptr)
  {
    if (status_thread->joinable())
      status_thread->join();
    delete status_thread;
    status_thread = nullptr;
  }
  CONSOLE.Log("Stopped status thread");
  if (ws_conn_thread != nullptr)
  {
    if (ws_conn_thread->joinable())
      ws_conn_thread->join();
    delete ws_conn_thread;
    ws_conn_thread = nullptr;
  }
  CONSOLE.Log("Stopped reconnection thread");

  if (ws_cli != nullptr)
  {
    ws_cli->clearData();
    ws_cli->closeConnection();
    if (ws_cli_thread != nullptr)
    {
      if (ws_cli_thread->joinable())
        ws_cli_thread->join();
      delete ws_cli_thread;
      ws_cli_thread = nullptr;
    }
    delete ws_cli;
    ws_cli = nullptr;
  }
  CONSOLE.Log("Stopped connection");

  if (dump_file != NULL)
  {
    dump_file->close();
    dump_file = NULL;
  }
  CONSOLE.Log("Closed dump file");

  for (auto &el : sockets)
  {
    el.second.sock->close_socket();
    if (el.second.thrd != nullptr)
    {
      CONSOLE.Log("Joining CAN thread [", el.second.name, "]");
      el.second.thrd->join();
      CONSOLE.Log("Joined CAN thread [", el.second.name, "]");
    }
  }
  CONSOLE.Log("Closed can socket");

  for (int i = 0; i < gps_loggers.size(); i++)
  {
    if (gps_loggers[i] == nullptr)
      continue;
    cout << i << endl;
    gps_loggers[i]->Kill();
    gps_loggers[i]->WaitForEnd();
    delete gps_loggers[i];
  }
  gps_loggers.resize(0);
  CONSOLE.Log("Closed gps loggers");

  // if(chimera != nullptr)
  // {
  //   chimera->clear_serialized();
  //   chimera->close_all_files();
  //   delete chimera;
  //   chimera = nullptr;
  // }
  primary_pack.Clear();
  secondary_pack.Clear();
  if (tel_conf.generate_csv)
  {
    // Close all csv files and the dump file
    // chimera->close_all_files();
    for (int i = 0; i < primary_NUMBER_OF_MESSAGES; i++)
    {
      if (primary_files[i] != NULL)
      {
        fclose(primary_files[i]);
        primary_files[i] = NULL;
      }
    }
    for (int i = 0; i < secondary_NUMBER_OF_MESSAGES; i++)
    {
      if (secondary_files[i] != NULL)
      {
        fclose(secondary_files[i]);
        secondary_files[i] = NULL;
      }
    }
  }
  CONSOLE.Log("Deleted vehicle");

  msgs_counters.clear();
  msgs_per_second.clear();
  timers.clear();

  ws_conn_state = ConnectionState_::NONE;
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
  if (path_exists(path))
  {
    if (LoadStruct(tel_conf, path))
      CONSOLE.Log("Loaded telemetry config");
    else
      EmitError(TEL_LOAD_CONFIG_TELEMETRY);
  }
  else
  {
    CONSOLE.Log("Created: " + path);
    tel_conf.can_devices = {can_devices_o{"can0", "primary"}};
    tel_conf.generate_csv = false;
    tel_conf.ws_enabled = true;
    tel_conf.ws_send_sensor_data = true;
    tel_conf.ws_send_rate = 500;
    tel_conf.ws_downsample = true;
    tel_conf.ws_downsample_mps = 50;
    tel_conf.ws_server_url = "ws://eagle-telemetry-server.herokuapp.com";
    SaveStruct(tel_conf, path);
  }

  path = HOME_PATH + "/session_config.json";
  if (path_exists(path))
  {
    if (LoadStruct(sesh_config, path))
      CONSOLE.Log("Loaded session config");
    else
      EmitError(TEL_LOAD_CONFIG_SESSION);
  }
  else
  {
    CONSOLE.Log("Created: " + path);
    SaveStruct(sesh_config, path);
  }
}
void TelemetrySM::SaveAllConfig()
{
  string path = " ";
  path = HOME_PATH + "/telemetry_config.json";
  CONSOLE.Log("Saving new tel config");
  SaveStruct(tel_conf, path);
  CONSOLE.Log("Done");
  CONSOLE.Log("Saving new sesh config");
  path = HOME_PATH + "/session_config.json";
  SaveStruct(sesh_config, path);
  CONSOLE.Log("Done");
}

void TelemetrySM::SaveStat()
{
  can_stat.Average_Frequency_Hz = double(can_stat.Messages) / can_stat.Duration_seconds;
  SaveStruct(can_stat, CURRENT_LOG_FOLDER + "/CAN_Stat.json");

  sesh_config.Canlib_Version = primary_IDS_VERSION;
  SaveStruct(sesh_config, CURRENT_LOG_FOLDER + "/Session.json");
}

void TelemetrySM::CreateHeader(string &out)
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
void TelemetrySM::CreateFolderName(string &out)
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
void TelemetrySM::LogCan(const CAN_Message &message)
{
  if (dump_file == NULL || !dump_file->is_open())
    CONSOLE.LogError("candump file not opened");
  else
    (*dump_file) << message.timestamp << " " << message.receiver_name << " " << CanMessage2Str(message.frame) << "\n";
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
string TelemetrySM::CanMessage2Str(const can_frame &msg)
{
  // Format message as ID#<payload>
  // Hexadecimal representation
  char buff[22];
  static char id[3];
  static char val[2];
  get_hex(id, msg.can_id, 3);
  sprintf(buff, "%s#", id);
  for (int i = 0; i < msg.can_dlc; i++)
  {
    get_hex(val, msg.data[i], 2);
    strcat(buff, val);
  }
  return string(buff);
}

void TelemetrySM::OpenLogFolder(const string &path)
{
  CONSOLE.Log("Output Folder ", path);
  if (!path_exists(path))
  {
    CONSOLE.LogWarn("Creating log folder");
    if (!create_directory(path))
      EmitError(TEL_LOG_FOLDER);
  }
}

void TelemetrySM::OpenCanSocket()
{
  sockets.clear();
  for (auto dev : tel_conf.can_devices)
  {
    CONSOLE.Log("Opening Socket ", dev.sock);
    CAN_Socket &new_can = sockets[dev.name];
    new_can.name = dev.name;
    new_can.sock = new Can(dev.sock.c_str(), &new_can.addr);
    new_can.thrd = nullptr;
    if (new_can.sock->open_socket() < 0)
    {
      CONSOLE.LogWarn("Failed opening socket: ", dev.name, " ", dev.sock);
      EmitError(TEL_CAN_SOCKET_OPEN);
      return;
    }
    CONSOLE.Log("Opened Socket: ", dev.name);
    CONSOLE.Log("Starting CAN thread");
    new_can.thrd = new thread(&TelemetrySM::CanReceive, this, &new_can);
    CONSOLE.Log("Started CAN thread");
  }
}

void TelemetrySM::SetupGps()
{
  CONSOLE.Log("Initializing gps instances");
  // Setup of all GPS devices
  for (size_t i = 0; i < tel_conf.gps_devices.size(); i++)
  {
    string dev = tel_conf.gps_devices[i];
    string mode = tel_conf.gps_mode[i];
    bool enabled = tel_conf.gps_enabled[i];
    if (dev == "")
      continue;

    CONSOLE.Log("Initializing ", dev, mode, enabled);

    int id;
    // if(i == 0)
    //   id = chimera->gps1->get_id();
    // else if(i == 1)
    //   id = chimera->gps2->get_id();

    GpsLogger *gps = new GpsLogger(i, dev);
    gps->SetOutFName("gps_" + to_string(gps->GetId()));
    msgs_counters["gps_" + to_string(gps->GetId())] = 0;
    msgs_per_second["gps_" + to_string(gps->GetId())] = 0;
    if (mode == "file")
      gps->SetMode(MODE_FILE);
    else
      gps->SetMode(MODE_PORT);

    /////////////////
    // LAP COUNTER //
    /////////////////
    lp = lc_init(NULL); // initialization with default settings
    lp_inclination = lc_init(NULL);
    previousX = -1;
    previousY = -1;

    gps->SetCallback(bind(&TelemetrySM::OnGpsLine, this, std::placeholders::_1, std::placeholders::_2));

    gps_loggers.push_back(gps);
  }
  CONSOLE.Log("Done");
}
// Callback, fires every time a line from a GPS is received
void TelemetrySM::OnGpsLine(int id, string line)
{
  Gps *gps;

  // Selecting one of chimera GPS
  // if(id == chimera->gps1->get_id())
  //   gps = chimera->gps1;
  // else if(id == chimera->gps2->get_id())
  //   gps = chimera->gps2;
  // else
  //   return;

  // Parsing GPS data
  int ret = 0;
  try
  {
    // ret = chimera->parse_gps(gps, get_timestamp(), line);
  }
  catch (std::exception e)
  {
    CONSOLE.LogError("GPS parse error: ", line);
    return;
  }

  // If parsing was successfull
  // save parsed data into gps file
  if (ret == 1)
  {
    // lapCounter
    point.x = gps->latitude;
    point.y = gps->longitude;

    if (!(point.x == previousX && point.y == previousY))
    {
      previousX = point.x;
      previousY = point.y;

      if (point.x == 0 && point.y == 0)
      {
        // return
      }

      if (lc_eval_point(lp, &point, lp_inclination))
      {
        // è passato un giro
        // PHIL: arriva al volante?
      }
    }

    msgs_counters["gps_" + to_string(id)]++;

    unique_lock<mutex> lck(mtx);

    if (GetCurrentState() == ST_RUN && tel_conf.generate_csv)
    {
      (*gps->files[0]) << gps->get_string(",") + "\n"
                       << flush;
    }
    // if(tel_conf.ws_send_sensor_data)
    //   chimera->serialize_device(gps);
  }
  else
  {
    // cout << ret << " " << line << endl;
  }
}

void TelemetrySM::ConnectToWS()
{
  ws_cli->addOnOpen(bind(&TelemetrySM::OnOpen, this, std::placeholders::_1));
  ws_cli->addOnClose(bind(&TelemetrySM::OnClose, this, std::placeholders::_1, std::placeholders::_2));
  ws_cli->addOnError(bind(&TelemetrySM::OnError, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

  // sends sensors data only if connected
  data_thread = new thread(&TelemetrySM::SendWsData, this);
  status_thread = new thread(&TelemetrySM::SendStatus, this);
  while (kill_threads.load() == false)
  {
    usleep(1000000);
    if (!tel_conf.ws_enabled)
      continue;
    if (ws_conn_state == ConnectionState_::CONNECTED || ws_conn_state == ConnectionState_::CONNECTING)
      continue;

    ws_cli->init(tel_conf.ws_server_url, "", 0);
    // ws_cli->init("192.168.42.88", "3000", 0);
    ws_cli->addOnMessage(bind(&TelemetrySM::OnMessage, this, std::placeholders::_1, std::placeholders::_2));
    ws_cli_thread = ws_cli->start();
    if (ws_cli_thread == nullptr)
    {
      CONSOLE.ErrorMessage("Failed connecting to server: " + tel_conf.ws_server_url);
    }
    // Login as telemetry
    ws_cli->clearData();
    ws_cli->setData(GenericMessage("{\"identifier\":\"telemetry\"}"));
    ws_conn_state = ConnectionState_::CONNECTING;
  }
  CONSOLE.LogError("KILLED");
}

void TelemetrySM::OnMessage(const int &id, const GenericMessage &msg)
{
  Document req;
  StringBuffer sb;
  Writer<StringBuffer> w(sb);
  rapidjson::Document::AllocatorType &alloc = req.GetAllocator();

  Document ret;
  StringBuffer sb2;
  Writer<StringBuffer> w2(sb2);
  rapidjson::Document::AllocatorType &alloc2 = ret.GetAllocator();

  ParseResult ok = req.Parse(msg.data.c_str(), msg.data.size());
  if (!ok || !req.HasMember("type"))
  {
    return;
  }
  if (req["type"] == "telemetry_set_sesh_config")
  {
    if (req["data"].HasMember("Pilot") &&
        req["data"].HasMember("Circuit") &&
        req["data"].HasMember("Configuration") &&
        req["data"].HasMember("Race"))
    {
      sesh_config.Configuration = req["data"]["Configuration"].GetString();
      sesh_config.Race = req["data"]["Race"].GetString();
      if (string(req["data"]["Pilot"].GetString()) != "")
        sesh_config.Pilot = req["data"]["Pilot"].GetString();
      if (string(req["data"]["Circuit"].GetString()) != "")
        sesh_config.Circuit = req["data"]["Circuit"].GetString();

      SaveAllConfig();
    }
    else
    {
      CONSOLE.LogWarn("Telemetry set session config (Wrong members)");
    }
  }
  if (req["type"] == "telemetry_set_tel_config")
  {
    telemetry_config buffer;
    Deserialize(buffer, req["data"]);
    // try
    // {
    //   Deserialize(buffer, j["data"]);
    // }
    // catch(const std::exception& e)
    // {
    //   CONSOLE.LogWarn("Failed parsing telemetry config (from ws) ", msg.data);
    // }
    tel_conf = buffer;

    SaveAllConfig();
  }
  else if (req["type"] == "ping")
  {
    CONSOLE.DebugMessage("Requested ping");
    ret.SetObject();
    ret.AddMember("type", Value().SetString("server_answer_ping"), alloc2);
    ret.AddMember("time", (get_timestamp() - req["time"].GetDouble()), alloc2);
    ret.Accept(w2);

    ws_cli->setData(GenericMessage(sb2.GetString()));
  }
  else if (req["type"] == "telemetry_get_config")
  {
    CONSOLE.DebugMessage("Requested configs");

    Document d;
    Serialize(d, tel_conf);
    d.Accept(w2);
    string conf1 = d.GetString();
    d.Clear();
    Serialize(d, tel_conf);
    d.Accept(w2);
    string conf2 = d.GetString();

    ret.SetObject();
    ret.AddMember("type", Value().SetString("telemetry_config"), alloc2);
    ret.AddMember("telemetry_config", Value().SetString(conf1.c_str(), conf1.size(), alloc2), alloc2);
    ret.AddMember("session_config", Value().SetString(conf2.c_str(), conf2.size(), alloc2), alloc2);
    ret.Accept(w2);

    ws_cli->setData(GenericMessage(sb2.GetString()));
    CONSOLE.Log("Done config");
  }
  else if (req["type"] == "telemetry_kill")
  {
    CONSOLE.Log("Requested Kill (from ws)");
    exit(0);
  }
  else if (req["type"] == "telemetry_reset")
  {
    CONSOLE.Log("Requested Reset (from ws)");
    wsRequestState = ST_UNINITIALIZED;
  }
  else if (req["type"] == "telemetry_start")
  {
    CONSOLE.Log("Requested Start (from ws)");
    wsRequestState = ST_RUN;
  }
  else if (req["type"] == "telemetry_stop")
  {
    CONSOLE.Log("Requested Stop (from ws)");
    wsRequestState = ST_STOP;
  }
  else if (req["type"] == "telemetry_action_zip_logs")
  {
    CONSOLE.Log("Requested action: telemetry_action_zip_logs");
    unique_lock<mutex> lck(mtx);
    action_string = "cd /home/pi/telemetry/python && python3 zip_logs.py all";
  }
  else if (req["type"] == "telemetry_action_zip_and_move")
  {
    CONSOLE.Log("Requested action: telemetry_action_zip_and_move");
    unique_lock<mutex> lck(mtx);
    action_string = "cd /home/pi/telemetry/python && python3 zip_and_move.py all";
  }
  else if (req["type"] == "telemetry_action_raw")
  {
    if (req.HasMember("data"))
    {
      CONSOLE.Log("Requested action:", req["data"].GetString());
      unique_lock<mutex> lck(mtx);
      action_string = req["data"].GetString();
    }
  }
  else if (req["type"] == "telemetry_send_can_message")
  {
    CONSOLE.Log("Requested to send a can message to car");
    if ((req.HasMember("id") && req.HasMember("payload")) &&
        (req["id"].IsInt() && req["payload"].IsArray()))
    {
      auto payload = req["payload"].GetArray();
      if (payload.Size() > 0 && payload.Size() <= 8 && payload[0].IsInt())
      {
        char msg[8];
        for (int i = 0; i < payload.Size(); i++)
          msg[i] = (char)payload[i].GetInt();
        if (sockets.find("primary") != sockets.end())
          sockets["primary"].sock->send(req["id"].GetInt(), msg, 8);
      }
      else
      {
        CONSOLE.LogWarn("CAN message malformed (from ws)");
      }
    }
    else
    {
      CONSOLE.LogWarn("CAN message malformed (from ws)");
    }
  }
}

void TelemetrySM::SendStatus()
{
  uint8_t msg_data[8];
  int is_in_run;
  primary_message_TLM_STATUS status;
  while (kill_threads.load() == false)
  {
    if (GetCurrentState() == ST_RUN)
      is_in_run = 1;
    else
      is_in_run = 0;
    primary_serialize_TLM_STATUS(msg_data,
                                 0,
                                 0,
                                 primary_RaceType::primary_RaceType_AUTOCROSS,
                                 is_in_run ? primary_Toggle_ON : primary_Toggle_OFF);
    if (sockets.find("primary") != sockets.end())
      sockets["primary"].sock->send(primary_id_TLM_STATUS, (char *)msg_data, primary_TLM_STATUS_SIZE);

    string str;
    for (auto el : msgs_counters)
    {
      msgs_per_second[el.first] = msgs_counters[el.first];
      msgs_counters[el.first] = 0;
      str += el.first + " " + to_string(msgs_per_second[el.first]) + " ";
    }
    CONSOLE.Log("Status", StatesStr[GetCurrentState()], "MSGS per second: " + str);

    if (tel_conf.ws_enabled && ws_conn_state == ConnectionState_::CONNECTED)
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
      for (auto el : msgs_per_second)
      {
        val.AddMember(Value().SetString(el.first.c_str(), alloc), el.second, alloc);
      }
      d.AddMember("msgs_per_second", val, alloc);
#ifdef WITH_CAMERA
      auto cam_state = camera.StatesStr[camera.GetCurrentState()];
      auto cam_error = CamErrorStr[camera.GetError()];
      d.AddMember("camera_status", Value().SetString(cam_state.c_str(), cam_state.size(), alloc), alloc);
      d.AddMember("camera_error", Value().SetString(cam_error.c_str(), cam_error.size(), alloc), alloc);
#endif // WITH_CAMERA

      d.AddMember("cpu_total_load", cpu_total_load_value(), alloc);
      d.AddMember("cpu_process_load", cpu_process_load_value(), alloc);
      d.AddMember("mem_process_load", mem_process_load_value(), alloc);

      d.Accept(w);

      ws_cli->setData(GenericMessage(sb.GetString()));
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

  string primary_serialized;
  string secondary_serialized;
  bool primary_ok;
  bool secondary_ok;
  while (kill_threads.load() == false)
  {
    while (ws_conn_state != ConnectionState_::CONNECTED)
      usleep(500000);
    while (ws_conn_state == ConnectionState_::CONNECTED)
    {
      if (kill_threads.load() == true)
        break;
      usleep(1000 * tel_conf.ws_send_rate);

      if (!tel_conf.ws_send_sensor_data)
        continue;

      unique_lock<mutex> lck(mtx);

      primary_ok = primary_pack.SerializeToString(&primary_serialized);
      secondary_ok = secondary_pack.SerializeToString(&secondary_serialized);

      if (primary_ok == false && secondary_ok == false)
        continue;

      sb.Clear();
      w.Reset(sb);
      d.SetObject();
      d.AddMember("type", Value().SetString("update_data"), alloc);
      d.AddMember("timestamp", get_timestamp(), alloc);
      if (primary_ok)
        d.AddMember("primary", Value().SetString(primary_serialized.c_str(), primary_serialized.size(), alloc), alloc);
      if (secondary_ok)
        d.AddMember("secondary", Value().SetString(secondary_serialized.c_str(), secondary_serialized.size(), alloc), alloc);
      d.Accept(w);

      ws_cli->setData(GenericMessage(sb.GetString()));
      primary_pack.Clear();
      secondary_pack.Clear();
    }
  }
}

void TelemetrySM::ActionThread()
{
  string cmd_copy = "";
  while (kill_threads.load() == false)
  {
    usleep(1000);
    {
      unique_lock<mutex> lck(mtx);
      if (action_string == "")
        continue;
      cmd_copy = action_string;
    }

    try
    {
      CONSOLE.Log("Starting command:", cmd_copy);
      string status = "Action: " + cmd_copy + " ----> started";
      ws_cli->setData(GenericMessage("{\"type\":\"action_result\",\"data\":\"" + status + "\"}"));

      int ret = system(cmd_copy.c_str());
      if (ret == 0)
      {
        status = "Action: " + cmd_copy + " ----> successful";
        CONSOLE.Log(status);
        ws_cli->setData(GenericMessage("{\"type\":\"action_result\",\"data\":\"" + status + "\"}"));
      }
      else
      {
        status = "Action: " + cmd_copy + " ----> failed with code: " + to_string(ret);
        CONSOLE.LogError(status);
        ws_cli->setData(GenericMessage("{\"type\":\"action_result\",\"data\":\"" + status + "\"}"));
      }
    }
    catch (exception e)
    {
      CONSOLE.LogError("Actions exception:", e.what());
    }

    action_string = "";
  }
}

void TelemetrySM::CanReceive(CAN_Socket *can)
{
  CAN_Message msg;
  msg.receiver_name = can->name;
  while (!kill_threads)
  {
    can->sock->receive(&msg.frame);
    msg.timestamp = get_timestamp_u();
    {
      unique_lock<mutex> lck(can_mutex);
      messages_queue.push(msg);
    }
    can_cv.notify_all();
  }
}

void TelemetrySM::ProtoSerialize(const int &can_network, const uint64_t &timestamp, const can_frame &msg, const int &dev_idx)
{
  // Serialize with protobuf if websocket is enabled
  if (tel_conf.ws_enabled == false || tel_conf.ws_send_sensor_data == false)
    return;
  if (tel_conf.ws_downsample == true)
  {
    if (can_network == 0)
    {
      if ((1.0e6 / tel_conf.ws_downsample_mps) < (timestamp - timers["primary_" + to_string(dev_idx)]))
      {
        timers["primary_" + to_string(dev_idx)] = timestamp;
        primary_proto_serialize_from_id(msg.can_id, &primary_pack, &primary_devs);
      }
    }
    else if (can_network == 1)
    {
      if ((1.0e6 / tel_conf.ws_downsample_mps) < (timestamp - timers["secondary_" + to_string(dev_idx)]))
      {
        timers["secondary_" + to_string(dev_idx)] = timestamp;
        secondary_proto_serialize_from_id(msg.can_id, &secondary_pack, &secondary_devs);
      }
    }
  }
  else
  {
    if (can_network == 0)
    {
      primary_proto_serialize_from_id(msg.can_id, &primary_pack, &primary_devs);
    }
    else if (can_network == 1)
    {
      secondary_proto_serialize_from_id(msg.can_id, &secondary_pack, &secondary_devs);
    }
  }
}

void TelemetrySM::OnOpen(const int &id)
{
  CONSOLE.Log("WS opened");
  ws_conn_state = ConnectionState_::CONNECTED;
}
void TelemetrySM::OnClose(const int &id, const int &num)
{
  CONSOLE.LogError("WS Closed <", num, ">");
  ws_conn_state = ConnectionState_::CLOSED;
}
void TelemetrySM::OnError(const int &id, const int &num, const string &err)
{
  CONSOLE.LogError("WS Error <", num, ">", "message: ", err);
  ws_conn_state = ConnectionState_::FAIL;
}