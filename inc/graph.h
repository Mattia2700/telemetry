#pragma once

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>

#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>

using namespace std;
using namespace cv;


#include "renderer.h"

static vector<Scalar> colors{
  Scalar(25 , 225, 255, 255),
  Scalar(48 , 130, 245, 255),
  Scalar(240, 240, 70 , 255),
  Scalar(230, 50 , 240, 255),
  Scalar(195, 255, 170, 255),
  Scalar(255, 190, 220,255),
  Scalar(75 , 25 , 230, 255),
  Scalar(60 , 245, 210, 255),
  Scalar(40 , 110, 170, 255),
  Scalar(128, 128, 0  , 255),
  Scalar(0  , 128, 128, 255),
};

struct Box
{
  int x;
  int y;
  int w;
  int h;
};


class Graph : public UIElement
{
public:
  Graph(string name = ""): UIElement(name), auto_resize(false)
  {
    std::srand(time(nullptr));
  }

  void PushData(vector<double>& x, vector<vector<double>>& ys);
  void Draw(Mat* img);

  void SetMaxLenght(int);
  void SetScale(double, double);
  void AutoResize(bool);

  void SetPosition(Box box);
  void SetOffsetX(double x){offset_x = x;};

  void SetLabels(vector<string> vec);

private:
  void CheckLength(vector<double>* to_check);
  void DrawAxes(Mat* img);
  Scalar GenerateColor();

  virtual void CommandMode(char);

  // Vector containing x axis values
  vector<double> x;
  // Vector containing y axis values
  vector<vector<double>> ys;
  // value to substract from x axis values
  double offset_x;

  vector<string> labels;

  int max_length = 10000;

  float scaleX;
  float scaleY;

  bool auto_resize;

  Box position;
  vector<Scalar> line_colors;
  vector<bool> show_line;
  int label_hover_idx=-1;
};
