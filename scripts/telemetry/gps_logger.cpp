#include "gps_logger.h"


GpsLogger::GpsLogger(string device)
{
  m_Device = device;

  m_Running = false;
  m_Thread = new thread(&GpsLogger::Run, this);
}

void GpsLogger::SetOutputFolder(string folder)
{
  m_Folder = folder;
}

void GpsLogger::Start()
{
  m_Running = true;
  cv.notify_all();
}

void GpsLogger::Stop()
{
  m_Running = false;
  cv.notify_all();
}

void GpsLogger::Kill()
{
  m_Kill = true;
  m_Running = false;
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

  if (m_Serial->open_file() < 0)
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

    if(m_Kill)
      break;

    if(!OpenDevice()){
      m_Running = false;
      continue;
    }

    string fname = m_Folder + "/" + "gps_telemetry.log";
    std::ofstream gps(fname);
    string line;
    while(m_Running)
    {
      line = m_Serial->read_line('\n');
      line = "(" + to_string(GetTimestamp()) + ")" + "\t" + line + "\n";
      gps << line;
    }
    gps.close();
  }
}


double GpsLogger::GetTimestamp()
{
  return duration_cast<duration<double, milli>>(system_clock::now().time_since_epoch()).count() / 1000;
}
