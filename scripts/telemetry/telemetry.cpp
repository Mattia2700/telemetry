#include "telemetry.h"

int main()
{

  if (USE_GPS)
  {
    s = serial(GPS_DEVICE);
    if (s.open_file() < 0)
    {
      cout << get_colored("Failed opeing " + string(GPS_DEVICE), 1) << endl;
      USE_GPS = 0;
    }
    else
    {
      cout << get_colored("Opened " + string(GPS_DEVICE), 6) << endl;
    }
  }

  HOME_PATH = getenv("HOME");
  FOLDER_PATH = "/Desktop/logs";
  cout << get_colored("Output Folder" + HOME_PATH + FOLDER_PATH, 2) << endl;
  if (!path_exists(HOME_PATH + FOLDER_PATH))
  {
    cout << get_colored("Failed, changing folder... ", 1) << endl;
    FOLDER_PATH = "/logs";
    cout << get_colored("Output Folder" + HOME_PATH + FOLDER_PATH, 2) << endl;
    if (!path_exists(HOME_PATH + FOLDER_PATH))
    {
      cout << get_colored("Folder not found!", 1) << endl;
      cout << FOLDER_PATH << endl;
      return 0;
    }
  }

  FOLDER_PATH = HOME_PATH + FOLDER_PATH;

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

  Chimera chimera;

  // indices for PILOTS, RACES, CIRCUITS
  int i1 = 0, i2 = 0, i3 = 0;
  while (true)
  {
    killThread = true;
    messages_count = 0;
    // Send telemetry status IDLE
    msg_data[0] = 0;
    can->send(0x99, (char *)msg_data, 1);

    // Set CAN filter to accept only 0xA0 id
    rfilter.can_id = 0xA0;
    rfilter.can_mask = 0b11111111111;
    can->set_filters(rfilter);

    while (true)
    {
      can->receive(&message);
      if (message.can_id == 0xA0 && message.can_dlc >= 2)
      {
        // Start Message
        if (message.data[0] == 0x66 && message.data[1] == 0x01)
        {
          // If contains some payload use to setup pilots races and circuits
          if (message.can_dlc >= 5)
          {
            i1 = message.data[2];
            i2 = message.data[3];
            i3 = message.data[4];
          }
          else
          {
            i1 = 0;
            i2 = 0;
            i3 = 0;
          }
          cout << "Status: " << get_colored("Running", 3) << "\r" << flush;
          // let the GPS thread run
          killThread = false;
          break;
        }
      }
    }
    // To remove a filter set the mask to 0
    // So every id will pass the mask
    rfilter.can_id = 0x000;
    rfilter.can_mask = 0b00000000000;
    can->set_filters(rfilter);

    // Check index range
    if (i1 >= PILOTS.size())
      i1 = 0;
    if (i2 >= RACES.size())
      i2 = 0;
    if (i3 >= CIRCUITS.size())
      i3 = 0;

    // Get human readable date
    time_t date = time(0);
    char *date_c = ctime(&date);

    // Insert header at top of the file
    string header = "\r\n\n";
    header += "*** EAGLE-TRT\r\n";
    header += "*** Telemetry Log File\r\n";
    header += "*** " + string(date_c);
    header += "\r\n";
    header += "*** Pilot: " + PILOTS[i1] + "\r\n";
    header += "*** Race: " + RACES[i2] + "\r\n";
    header += "*** Circuit: " + CIRCUITS[i3];
    header += "\n\n\r";

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
    subfolder << PILOTS[i1];
    subfolder << "_";
    subfolder << RACES[i2];

    // get absolute path of folder
    string folder = FOLDER_PATH + "/" + subfolder.str();
    create_directory(folder);

    // string can_fname = folder + "/" + "candump.log";
    chimera.add_filenames(folder, ".csv");
    chimera.open_all_files();
    chimera.write_all_headers(0);

    string gps_fname = folder + "/" + "gps_telemetry.log";

    if (USE_GPS)
    {
      // spawn GPS logger thread
      delete gps_thread;
      gps_thread = new thread(log_gps, gps_fname, header);
    }

    double timestamp = get_timestamp();
    // Use this timer to send status messages
    double t_start = timestamp;
    double log_start_t = timestamp;
    double log_duration;
    while (true)
    {
      can->receive(&message);
      messages_count++;

      timestamp = get_timestamp();

      modifiedDevices = chimera.parse_message(timestamp, message.can_id, message.data, message.can_dlc);
      for (auto modified : modifiedDevices)
      {
        *modified->files[0] << modified->get_string(";") + "\n";
      }

      if (message.can_id == 0xA0 && message.can_dlc >= 2)
      {
        // Stop message
        if (message.data[0] == 0x66 && message.data[1] == 0x00)
        {
          log_duration = timestamp - log_start_t;
          cout << "Status: " << get_colored("Stopped", 1) << "\r" << flush;
          chimera.close_all_files();
          killThread = true;
          break;
        }
      }

      if (timestamp - t_start > 0.2)
      {
        // Send telemetry status (RUN)
        msg_data[0] = 1;
        can->send(0x99, (char *)msg_data, 1);
        t_start = timestamp;
      }
    }

    {
      std::unique_lock<std::mutex> lck(mtx);
      st["Date"] = date_c;

      st["Pilot"] = PILOTS[i1];
      st["Race"] = RACES[i2];
      st["Circuit"] = CIRCUITS[i3];

      st["Data"]["CAN"]["Messages"] = messages_count;
      st["Data"]["CAN"]["Average Frequency (Hz)"] = int(messages_count / log_duration);
      st["Data"]["CAN"]["Duration (seconds)"] = log_duration;

      std::ofstream stat_f(folder + "/TelemetryInfo.json");
      stat_f << st.dump(2);
      stat_f.close();
    }
  }
  return 0;
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

void log_gps(string fname, string header)
{
  // Use this mutex only for wring in json stat
  std::unique_lock<std::mutex> lck(mtx);
  std::ofstream gps(fname);

  if (header != "")
    gps << header << flush;

  int count = 0;
  auto t0 = high_resolution_clock::now();
  while (!killThread)
  {
    string line = s.read_line('\n');
    line = "(" + to_string(get_timestamp()) + ")" + "\t" + line + "\n";
    gps << line;
    count++;
  }

  gps.close();

  double dt = duration<double, milli>(high_resolution_clock::now() - t0)
                  .count() /
              1000;

  st["Data"]["GPS"]["Messages"] = count;
  st["Data"]["GPS"]["Average Frequency [Hz]"] = int(count / dt);
  st["Data"]["GPS"]["Duration [seconds]"] = dt;
}
