#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <chrono>
#include <unordered_map>

#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "devices.pb.h"
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/json_util.h>

#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

using namespace std;
using namespace cv;
using namespace std::chrono;
using namespace google::protobuf;
using namespace google::protobuf::util;

struct Box
{
  int x;
  int y;
  int w;
  int h;
};

struct Sate_t
{
  double timestamp;
  string value;
};

class Data{};
class Page;

class ChimeraData : public Data
{
public:
  ChimeraData(devices::Chimera* chim): data(chim){};

  void SetData(devices::Chimera* chim)
  {
    data = chim;
  };

  // void PushData(devices::Chimera* chim);

  devices::Chimera* data = nullptr;

  // unordered_map<string, unordered_map<string, vector<double>>> sensor_values;
  // unordered_map<string, vector<Sate_t>> state_values;

private:
  // int max_count = 1000;
};


class Renderer
{
public:
  Renderer(int w, int h);
  void SetBackground(int R, int G, int B);

  void SetFrameRate(int);
  void Kill();
  void WaitForFinish();

  void Pause();
  void Start();
  void ToggleStartStop();

  void SetOnKeyPress(void (*func)(char& ))
  {
    on_key_press = func;
  };

  /**
  * Adds a page to vector of pages that can be rendered
  * @param *page pointer to page
  * @param idx index in wich the page is added -1 pushes at last position
  */
  void AddPage(Page* page, int idx=-1);
  /**
  * Removes a page from vector of pages that can be rendered
  * @param *page pointer to page (it does not delete the whole pointer)
  */
  void RemovePage(Page* page);
  /**
  * Removes a page t given index -1 means last position
  */
  void RemovePage(int idx=-1);

  /**
  * Returns page at index
  * @param idx index of page to be returned if -1 returns current
  */
  Page* GetPage(int idx=-1);

  void MoveLeft();
  void MoveRight();
  /**
  * Renders page at index
  * @param idx if -1 renders last page
  */
  void MoveToIndex(int idx=-1);
private:
  void Render();
  void DrawFooter(Mat*);

  Page* current_page = nullptr;
  vector<Page *> pages;
  int page_idx;

  int W, H;

  thread* render_thread = nullptr;

  Mat background;
  Mat render_image;

  float frame_rate;

  atomic<int> pause;
  atomic<bool> kill;

  void (*on_key_press)(char&) = nullptr;
};