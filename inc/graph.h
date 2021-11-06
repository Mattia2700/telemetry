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

using namespace std;
using namespace cv;


#include "renderer.h"

static vector<Scalar> colors{
  Scalar(25 , 255, 255, 255),
  Scalar(58 , 140, 255, 255),
  Scalar(250, 250, 80 , 255),
  Scalar(230, 50 , 240, 255),
  Scalar(195, 255, 170, 255),
  Scalar(255, 190, 220,255),
  Scalar(75 , 25 , 230, 255),
  Scalar(60 , 245, 210, 255),
  Scalar(40 , 110, 170, 255),
  Scalar(128, 128, 0  , 255),
  Scalar(0  , 128, 128, 255),
};


class Graph : public UIElement
{
public:
  Graph(string name = ""): UIElement(name), auto_resize(true)
  {
    std::srand(time(nullptr));
    text_size = 0.45;
    font = FONT_HERSHEY_SIMPLEX;
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

  double FindMaximum();

  virtual void CommandMode(char);

  // Vector containing x axis values
  vector<double> x;
  // Vector containing y axis values
  vector<vector<double>> ys;
  // value to substract from x axis values
  double offset_x;

  vector<string> labels;

  int max_length = 1000;

  float scaleX;
  float scaleY;

  bool auto_resize;

  Box position;
  vector<Scalar> line_colors;
  vector<bool> show_line;
  int label_hover_idx=-1;

  float text_size;
  HersheyFonts font;
};
