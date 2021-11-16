#pragma once

#include <ctime>
#include <chrono>
#include <stdio.h>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <iostream>
#include <exception>

#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

#include "utils.h"
#include "serial.h"

using namespace std;
using namespace std::chrono;


class GpsLogger
{
public:
  GpsLogger(string device);

  void SetOutputFolder(string folder);

  void Start();
  void Stop();
  void Kill();
  void WaitForEnd();

  bool IsRunning();

private:

  void Run();
  int OpenDevice();

  double GetTimestamp();

  string m_Device;
  string m_Folder;

  serial *m_Serial = nullptr;
  thread *m_Thread = nullptr;

  bool m_Running;
  bool m_Kill;

  mutex mtx;
  condition_variable cv;
  bool m_StateChnged;
};
