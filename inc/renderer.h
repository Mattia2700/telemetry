#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <iostream>

#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "devices.pb.h"
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/json_util.h>

#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

using namespace std;
using namespace cv;
using namespace google::protobuf;
using namespace google::protobuf::util;

class Data{};
class Page;

class ChimeraData : public Data
{
public:
  ChimeraData(devices::Chimera* chim): data(chim){}

  void SetData(devices::Chimera* chim)
  {
    data = chim;
  };

  devices::Chimera* data = nullptr;
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

  void MoveLeft();
  void MoveRight();
  /**
  * Renders page at index
  * @param idx if -1 renders last page
  */
  void MoveToIndex(int idx=-1);
private:
  void Render();

  Page* current_page = nullptr;
  vector<Page *> pages;
  int page_idx;

  thread* render_thread = nullptr;

  Mat background;
  Mat render_image;

  float frame_rate;

  atomic<int> pause;
  atomic<bool> kill;
};


class Page
{
public:
  Page(int w, int h):W(w),H(h)
  {
    background_color = cv::Scalar(0, 0, 0, 0);
    background = new Mat(h, w, CV_8UC4, background_color);
    instance_count ++;
  };
  ~Page()
  {
    instance_count --;
  }

  Mat* GetImage();
  void SetData(Data*);
  void SetBackground(int, int, int);

  virtual void Draw() = 0;

  static int instance_count;
protected:
  Mat* background;
  Data* current_data = nullptr;

  Scalar background_color;

  int W, H;

  mutex mtx;
};

class Page1 : public Page
{
public:
  Page1(int w, int h): Page(w, h){};
  virtual void Draw();
};
