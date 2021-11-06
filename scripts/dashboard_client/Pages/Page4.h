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

class Page4 : public Page
{
public:
  Page4(string name, int w, int h);

  virtual void Draw();
private:

  void SetGraphData(ChimeraData*);

  int graph_line_size = 1;

  Graph* inv_l_graph;
  Graph* inv_r_graph;

  uint64_t frame_count=0;
};
