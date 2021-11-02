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


struct Box
{
  int x;
  int y;
  int w;
  int h;
};


class Graph
{
public:
  Graph(): auto_resize(false)
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

private:
  void CheckLength(vector<double>* to_check);
  void DrawAxes(Mat* img);
  Scalar GenerateColor();

  // Vector containing x axis values
  vector<double> x;
  // Vector containing y axis values
  vector<vector<double>> ys;
  // value to substract from x axis values
  double offset_x;

  int max_length = 1000;

  float scaleX;
  float scaleY;

  bool auto_resize;

  Box position;
  vector<Scalar> line_colors;
};
