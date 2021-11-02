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

class Page3 : public Page
{
public:
  Page3(string name, int w, int h);

  virtual void Draw();
private:

  void SetGraphData(ChimeraData*);

  int graph_line_size = 1;

  Graph* bms_graph;
  Graph* inv_graph;

  uint64_t frame_count=0;
};
