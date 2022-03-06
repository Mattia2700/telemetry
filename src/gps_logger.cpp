#include "gps_logger.h"

int GpsLogger::instance_id = 0;

GpsLogger::GpsLogger(string device)
{
  m_Device = device;

  m_FName = "gps";
  m_Mode = MODE_PORT;
  m_Kill = false;
  m_LogginEnabled = false;
  m_Running = false;
  m_StateChanged = false;

  id = instance_id;
  GpsLogger::instance_id++;

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

void GpsLogger::SetCallback(void (*f)(int, string))
{
  m_OnNewLine = f;
}

void GpsLogger::StartLogging()
{
  unique_lock<mutex> lck(logger_mtx);

  m_GPS      = new std::ofstream(m_Folder + "/" + m_FName + ".log");
  m_StatFile = new std::ofstream(m_Folder + "/" + m_FName + ".json");

  if(m_Header != "")
    (*m_GPS) << m_Header << endl;

  stat.delta_time = GetTimestamp();
  stat.msg_count = 0;

  m_LogginEnabled = true;
  m_Running = true;
  m_StateChanged = true;
  cv.notify_all();
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
  m_Thread->join();
}

int GpsLogger::OpenDevice()
{
  if(m_Serial == nullptr)
    m_Serial = new serial(m_Device);

  int ret = 0;
  if(m_Mode == MODE_PORT)
    ret = m_Serial->open_port();
  else
    ret = m_Serial->open_file();

  if (ret < 0)
  {
    cout << "GPS Failed opeing " << m_Device << endl;
    return 0;
  }
  else
  {
    cout << "GPS Opened " << m_Device << endl;
    return 1;
  }
}

void GpsLogger::Run()
{
  string line;
  int fail_count = 0;

  while(!m_Kill)
  {
    unique_lock<mutex> lck(mtx);
    while(!m_Running)
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
    while(m_Running)
    {
      try
      {
        line = m_Serial->read_line('\n');
      }
      catch(std::exception e)
      {
        cout << "GPS Error " << e.what() << endl;
        fail_count ++;
        if(fail_count >= 20)
        {
          cout << "Failed 20 times readline" << endl;
          m_Running = false;
          fail_count = 0;
          break;
        }
      }
      // When receiving continuously blank lines, sleep a bit
      if(line == ""){
        blank_lines_count ++;
        if(blank_lines_count > 10)
          usleep(1000);
        continue;
      }
      else{
        blank_lines_count = 0;
      }

      // Callback on new line
      if(m_OnNewLine != nullptr)
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
          cout << "GPS Error " << e.what() << endl;
          fail_count ++;
          if(fail_count >= 20)
          {
            cout << "Failed 20 times write to file" << endl;
            m_Running = false;
            fail_count = 0;
            break;
          }
        }
        stat.msg_count ++;
      }
    }

    if(!m_Running)
    {
      m_Serial->close_port();
    }
  }
}

double GpsLogger::GetTimestamp()
{
  return duration_cast<duration<double, milli>>(system_clock::now().time_since_epoch()).count() / 1000;
}


void GpsLogger::SaveStat()
{
  if(m_StatFile == nullptr){
    cout << "GPS " << id << " no stat file" << endl;
    return;
  }
  Document doc;
  StringBuffer json_ss;
  PrettyWriter<StringBuffer> writer(json_ss);
  rapidjson::Document::AllocatorType &alloc = doc.GetAllocator();

  doc.SetObject();
  time_t date = time(0);
  string human_date = string(ctime(&date));
  human_date.erase(human_date.size()-1, 1);

  doc.AddMember("Date", Value().SetString(StringRef(human_date.c_str())), alloc);

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
}
