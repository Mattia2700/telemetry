#include "gps_logger.h"


GpsLogger::GpsLogger(string device)
{
  m_Device = device;

  m_Mode = MODE_PORT;
  m_Running = false;
  m_StateChnged = false;
  m_Thread = new thread(&GpsLogger::Run, this);
}

void GpsLogger::SetMode(int mode)
{
  m_Mode = mode;
}

void GpsLogger::SetOutputFolder(string& folder)
{
  m_NewFolder = folder;
}

void GpsLogger::SetHeader(string& header)
{
  m_Header = header;
}

void GpsLogger::SetCallback(void (*f)(string))
{
  m_OnNewLine = f;
}

void GpsLogger::Start()
{
  m_Running = true;
  m_StateChnged = true;
  cv.notify_all();
}

void GpsLogger::Stop()
{
  m_Running = false;
  m_StateChnged = true;
  cv.notify_all();
}

void GpsLogger::Kill()
{
  m_Kill = true;
  m_Running = false;
  m_StateChnged = true;
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
  while(!m_Kill)
  {
    unique_lock<mutex> lck(mtx);
    while(!m_Running)
      cv.wait(lck);
    m_StateChnged = false;
    m_Folder = m_NewFolder;

    if(m_Kill)
      break;

    if(!OpenDevice()){
      m_Running = false;
      continue;
    }

    string fname = m_Folder + "/" + "gps_telemetry.log";
    std::ofstream gps(fname);
    string line;

    if(m_Header != "")
      gps << m_Header << endl;

    stat.delta_time = GetTimestamp();
    stat.msg_count = 0;
    // Checking if state is changes
    // Can happen that is requested Stop and immediately a start
    // In that case m_Running changes two times and the while(m_Running) doesn't
    // detects the double change.
    while(m_Running && !m_StateChnged)
    {
      line = m_Serial->read_line('\n');
      if(m_OnNewLine != nullptr)
        m_OnNewLine(line);
      line = "(" + to_string(GetTimestamp()) + ")" + "\t" + line + "\n";
      gps << line << flush;
      stat.msg_count ++;
    }
    stat.delta_time = GetTimestamp() - stat.delta_time;

    m_StateChnged = false;
    gps.close();
    m_Serial->close_port();

    SaveStat();
  }
}


double GpsLogger::GetTimestamp()
{
  return duration_cast<duration<double, milli>>(system_clock::now().time_since_epoch()).count() / 1000;
}


void GpsLogger::SaveStat()
{
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
  std::ofstream stat_f(m_Folder + "/GPS_Info.json");
  stat_f << json_ss.GetString();
  stat_f.close();

}
