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

class Page7 : public Page
{
public:
  Page7(string name, int w, int h);

  virtual void Draw();
private:

  void SetTextData(ChimeraData*);

  TextBox* text_box;

  uint64_t frame_count=0;
};
