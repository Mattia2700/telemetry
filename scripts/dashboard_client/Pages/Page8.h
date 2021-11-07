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

#include "renderer.h"
#include "text_box.h"

using namespace std;
using namespace cv;

class Page8 : public Page
{
public:
  Page8(string name, int w, int h);

  virtual int Draw();
private:

  int SetTextData(ChimeraData*);

  TextBox* value_text_box;
  TextBox* frequency_text_box;
  TextBox* ecu_state_text_box;
  TextBox* bms_state_text_box;
  TextBox* steer_state_text_box;

  uint64_t frame_count=0;

  string last_ecu_state = "";
  string last_bms_state = "";
  string last_steer_state = "";
};
