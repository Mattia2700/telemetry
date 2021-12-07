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

struct coord
{
  double x1;
  double y1;
  double x2;
  double y2;
};

class Page9 : public Page
{
public:
  Page9(string name, int w, int h);

  virtual int Draw();
private:

  int SetGPSData(ChimeraData*);

  coord vadena;
  Mat vadena_image;

  double latitude;
  double longitude;

};
