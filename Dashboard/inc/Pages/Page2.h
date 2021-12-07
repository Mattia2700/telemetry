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
#include "graph.h"

using namespace std;
using namespace cv;


class Page2 : public Page
{
public:
  Page2(string name, int w, int h);

  virtual int Draw();
private:

  void SetAccelGyroData(ChimeraData*);

  int graph_line_size = 1;

  Graph* accel_graph;
  Graph*  gyro_graph;

  uint64_t frame_count=0;
};
