#include "gps_logger.h"

GpsLogger::GpsLogger(int id_, string device)
{
  m_Device = device;

  m_FName = "gps";
  m_Mode = MODE_PORT;
  m_Kill = false;
  m_LogginEnabled = false;
  m_Running = false;
  m_StateChanged = false;

  id = id_;

  m_Thread = new thread(&GpsLogger::Run, this);
}

void GpsLogger::SetOutFName(const string& fname)
{
  m_FName = fname;
}

void GpsLogger::SetMode(int mode)
{
  m_Mode = mode;
}

void GpsLogger::SetOutputFolder(const string& folder)
{
  m_Folder = folder;
}

void GpsLogger::SetHeader(const string& header)
{
  m_Header = header;
}

void GpsLogger::SetCallback(void (*clbk)(int, string))
{
  m_OnNewLine = clbk;
}
void GpsLogger::SetCallback(std::function<void(int, string)> clbk)
{
  m_OnNewLine = clbk;
}

void GpsLogger::StartLogging()
{
  unique_lock<mutex> lck(logger_mtx);

  CONSOLE.Log("GPS", id, "Start Logging");

  m_GPS      = new std::ofstream(m_Folder + "/" + m_FName + ".log");
  m_StatFile = new std::ofstream(m_Folder + "/" + m_FName + ".json");

  if(!m_GPS->is_open())
    CONSOLE.LogError("GPS", id, "Error opening .log file", m_Folder + "/" + m_FName + ".log");
  if(!m_StatFile->is_open())
    CONSOLE.LogError("GPS", id, "Error opening .json file", m_Folder + "/" + m_FName + ".json");

  if(m_Header != "")
    (*m_GPS) << m_Header << endl;

  stat.delta_time = GetTimestamp();
  stat.msg_count = 0;

  m_LogginEnabled = true;
  m_Running = true;
  m_StateChanged = true;
  cv.notify_all();
  CONSOLE.Log("GPS", id, "Done");
}

void GpsLogger::StopLogging()
{
  unique_lock<mutex> lck(logger_mtx);

  stat.delta_time = GetTimestamp() - stat.delta_time;
  m_StateChanged = false;
  m_GPS->close();
  delete m_GPS;
  SaveStat();
  m_StatFile->close();
  delete m_StatFile;

  m_LogginEnabled = false;
  m_Running = false;
  m_StateChanged = true;
  cv.notify_all();
}

void GpsLogger::Start()
{
  m_Running = true;
  m_StateChanged = true;
  cv.notify_all();
}

void GpsLogger::Stop()
{
  m_Running = false;
  m_StateChanged = true;
  cv.notify_all();
}

void GpsLogger::Kill()
{
  m_Kill = true;
  m_Running = false;
  m_StateChanged = true;
  cv.notify_all();
}

void GpsLogger::WaitForEnd()
{
  cv.notify_all();
  m_Thread->join();
}

int GpsLogger::OpenDevice()
{
  CONSOLE.Log("GPS", id, "Opening device mode:", m_Mode);
  if(m_Serial == nullptr)
    m_Serial = new serial(m_Device);

  int ret = 0;
  if(m_Mode == MODE_PORT)
    ret = m_Serial->open_port();
  else
    ret = m_Serial->open_file();

  if (ret < 0)
  {
    CONSOLE.LogError("GPS", id, "Failed opening", m_Device);
    return 0;
  }
  else
  {
    CONSOLE.Log("GPS", id, "Opened ", m_Device);
    return 1;
  }
}

void GpsLogger::Run()
{
  string line;
  int line_fail_count = 0;
  int file_fail_count = 0;

  CONSOLE.Log("GPS", id, "Run thread");

  while(!m_Kill)
  {
    unique_lock<mutex> lck(mtx);
    while(!m_Running && !m_Kill)
      cv.wait(lck);


    m_StateChanged = false;
    if(m_Kill)
      break;

    if(m_Running)
    {
      if(!OpenDevice()){
        m_Running = false;
        continue;
      }
    }

    m_StateChanged = false;
    int blank_lines_count = 0;
    CONSOLE.Log("GPS",id,"Running");
    while(m_Running)
    {
      try
      {
        line = '$' + m_Serial->read_line('$');
      }
      catch(std::exception e)
      {
        CONSOLE.LogError("GPS", id, "Error", e.what());
        line_fail_count ++;
        if(line_fail_count >= 20)
        {
          CONSOLE.LogError("GPS", id, "Failed 20 times readline");
          m_Running = false;
          line_fail_count = 0;
          break;
        }
        continue;
      }
      // When receiving continuously blank lines, sleep a bit
      if(line == "$"){
        blank_lines_count ++;
        if(blank_lines_count > 10)
          usleep(1000);
        continue;
      }
      else{
        blank_lines_count = 0;
      }

      // Callback on new line
      if(m_OnNewLine)
        m_OnNewLine(id, line);


      if(m_StateChanged)
        break;

      if(m_LogginEnabled)
      {
        unique_lock<mutex> lck(logger_mtx);
        try
        {
          line = "(" + to_string(GetTimestamp()) + ")" + "\t" + line + "\n";
          if(m_GPS->is_open())
            (*m_GPS) << line << flush;
        }
        catch(std::exception e)
        {
          CONSOLE.LogError("GPS", id, "Error writing to file:", e.what());
          file_fail_count ++;
          if(file_fail_count >= 20)
          {
            CONSOLE.LogError("GPS", id, "Failed 20 times writing to file");
            m_Running = false;
            file_fail_count = 0;
            break;
          }
        }
        stat.msg_count ++;
      }
    }

    if(!m_Running)
    {
      CONSOLE.Log("GPS", id, "closing port");
      m_Serial->close_port();
      CONSOLE.Log("GPS", id, "Done");
    }
  }
}

double GpsLogger::GetTimestamp()
{
  return duration_cast<duration<double, milli>>(system_clock::now().time_since_epoch()).count() / 1000;
}


void GpsLogger::SaveStat()
{
  CONSOLE.Log("GPS", id, "Saving stat");
  if(m_StatFile == nullptr){
    CONSOLE.LogError("GPS", id, "no stat file");
    return;
  }
  Document doc;
  StringBuffer json_ss;
  PrettyWriter<StringBuffer> writer(json_ss);
  rapidjson::Document::AllocatorType &alloc = doc.GetAllocator();

  doc.SetObject();
  
  time_t date_ = time(0);
  struct tm ltm;
  localtime_r(&date_, &ltm);
  std::ostringstream ss;
  string date;
  string time;
  ss << std::put_time(&ltm, "%d_%m_%Y");
  date = ss.str();
  ss.str("");
  ss << std::put_time(&ltm, "%H:%M:%S");
  time = ss.str();
  doc.AddMember("Date", Value().SetString(StringRef(date.c_str())), alloc);
  doc.AddMember("Time", Value().SetString(StringRef(time.c_str())), alloc);

  Value val;
  val.SetObject();
  {
    val.AddMember("Messages", stat.msg_count, alloc);
    val.AddMember("Average Frequency (Hz)", int(double(stat.msg_count) / stat.delta_time), alloc);
    val.AddMember("Duration (seconds)", stat.delta_time, alloc);
  }
  doc.AddMember("GPS", val, alloc);

  doc.Accept(writer);
  (*m_StatFile) << json_ss.GetString();
  CONSOLE.Log("GPS", id, "Done");
}
