#include "telemetry.h"

int main()
{

  if(!open_log_folder())
    return 0;
  FOLDER_PATH = HOME_PATH + FOLDER_PATH;

  if(!open_can_socket())
    return 0;

  time_t date;
  string human_date;

  // sends status every 200 ms
  thread t(send_status);

  Chimera chimera;
  GpsLogger* gps = new GpsLogger(string(GPS_DEVICE));
  while (true)
  {
    messages_count = 0;

    // Set CAN filter to accept only 0xA0 id
    rfilter.can_id = 0xA0;
    rfilter.can_mask = 0b11111111111;
    can->set_filters(rfilter);

    wait_for_start();
    date = time(0);
    human_date = string(ctime(&date));
    human_date[human_date.size()-1] = ' ';
    cout << human_date << " -> " << get_colored("Running", 3) << endl;

    running = true;

    // To remove a filter set the mask to 0
    // So every id will pass the mask
    rfilter.can_id = 0x000;
    rfilter.can_mask = 0b00000000000;
    can->set_filters(rfilter);

    // Insert header at top of the file
    string header;
    create_header(header);

    string subfolder;
    create_folder_name(subfolder);

    // get absolute path of folder
    string folder = FOLDER_PATH + "/" + subfolder;
    create_directory(folder);

    gps->SetOutputFolder(folder);
    gps->Start();

    dump_file = new std::fstream(folder + "/" + "candump.log", std::fstream::out);

    chimera.add_filenames(folder, ".csv");
    chimera.open_all_files();
    chimera.write_all_headers(0);

    double timestamp = get_timestamp();
    double log_start_t = timestamp;
    double log_duration;
    while (true)
    {
      can->receive(&message);
      messages_count++;

      timestamp = get_timestamp();

      log_can(timestamp, message, *dump_file);

      modifiedDevices = chimera.parse_message(timestamp, message.can_id, message.data, message.can_dlc);
      for (auto modified : modifiedDevices)
      {
        *modified->files[0] << modified->get_string(";") + "\n";
      }

      // Stop message
      if (message.can_id  == 0xA0 && message.can_dlc >= 2 &&
          message.data[0] == 0x66 && message.data[1] == 0x00)
      {
        date = time(0);
        human_date = string(ctime(&date));
        human_date[human_date.size()-1] = ' ';
        cout << human_date << " -> " << get_colored("Stopped", 1) << endl;

        chimera.close_all_files();
        dump_file->close();
        delete dump_file;

        gps->Stop();

        running = false;
        break;
      }
    }
    log_duration = timestamp - log_start_t;
    /*
    {
      // Fill the struct
      logger_stat.date = human_date;
      logger_stat.pilot = PILOTS[i1].c_str();
      logger_stat.race = RACES[i2].c_str();
      logger_stat.circuit = CIRCUITS[i3].c_str();
      logger_stat.can_messages = messages_count;
      logger_stat.can_frequency = int(messages_count / log_duration);
      logger_stat.can_duration = log_duration;

      // Wait for GPS thread if was started
      if(USE_GPS == 1)
      	std::unique_lock<std::mutex> lck(mtx);

      doc.SetObject();

      // Add keys and string values
      doc.AddMember("Date", Value().SetString(StringRef(human_date)), alloc);
      doc.AddMember("Pilot", Value().SetString(StringRef(PILOTS[i1].c_str())), alloc);
      doc.AddMember("Race", Value().SetString(StringRef(RACES[i2].c_str())), alloc);
      doc.AddMember("Circuit", Value().SetString(StringRef(CIRCUITS[i3].c_str())), alloc);

      // Add subobject
      Value sub_obj;
      sub_obj.SetObject();
      {
        Value val;
        val.SetObject();
        {
          val.AddMember("Messages", messages_count, alloc);
          val.AddMember("Average Frequency (Hz)", int(messages_count / log_duration), alloc);
          val.AddMember("Duration (seconds)", log_duration, alloc);
        }
        sub_obj.AddMember("CAN", val, alloc);
      }
      doc.AddMember("Data", sub_obj, alloc);

      if (logger_stat.gps_messages == 0)
      {
        Value val;
        val.SetObject();
        {
          val.AddMember("Messages", logger_stat.gps_messages, alloc);
          val.AddMember("Average Frequency (Hz)", logger_stat.gps_frequency, alloc);
          val.AddMember("Duration (seconds)", logger_stat.gps_duration, alloc);
        }
        doc["Data"].AddMember("GPS", val, alloc);
      }

      doc.Accept(writer);
      std::ofstream stat_f(folder + "/TelemetryInfo.json");
      stat_f << json_ss.GetString();
      stat_f.close();
    }
    */
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
  cout << get_colored("Output Folder " + HOME_PATH + FOLDER_PATH, 2) << endl;
  if (!path_exists(HOME_PATH + FOLDER_PATH))
  {
    cout << get_colored("Failed, changing folder... ", 1) << endl;
    FOLDER_PATH = "/Desktop/logs";
    cout << get_colored("Output Folder " + HOME_PATH + FOLDER_PATH, 2) << endl;
    if (!path_exists(HOME_PATH + FOLDER_PATH))
    {
      cout << get_colored("Folder not found!", 1) << endl;
      cout << FOLDER_PATH << endl;
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
    cout << get_colored("Failed binding socket: " + string(CAN_DEVICE), 1) << endl;
    CAN_DEVICE = "can0";
    can = new Can(CAN_DEVICE, &addr);
    sock = can->open();
    if (sock < 0)
    {
      cout << get_colored("Failed binding socket: " + string(CAN_DEVICE), 1) << endl;
      cout << "Exiting" << endl;
      return 0;
    }
  }
  cout << get_colored("Opened Socket: " + string(CAN_DEVICE), 6) << endl;
  return 1;
}

void wait_for_start()
{
  while (true)
  {
    can->receive(&message);
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
      break;
    }
  }

  // Check index range
  if (config.pilot >= PILOTS.size())
    config.pilot = 0;
  if (config.race >= RACES.size())
    config.race = 0;
  if (config.circuit >= CIRCUITS.size())
    config.circuit = 0;
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
  out += "*** Pilot: " + PILOTS[config.pilot] + "\r\n";
  out += "*** Race: " + RACES[config.race] + "\r\n";
  out += "*** Circuit: " + CIRCUITS[config.circuit];
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
    msg_data[0] = running;
    can->send(0x99, (char *)msg_data, 1);
    usleep(200000);
  }
}
