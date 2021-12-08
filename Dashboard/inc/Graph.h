#pragma once

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include <GL/glew.h>

using namespace std;


#include "UIElement.h"
#include "Application.h"

#include <libOpenGLCustom/Renderer.h>
#include <libOpenGLCustom/Input.h>

static vector<glm::vec4> colors{
  {0.098f, 1.000f, 1.000f, 1.0f},
  {0.227f, 0.549f, 1.000f, 1.0f},
  {0.990f, 0.990f, 0.313f, 1.0f},
  {0.902f, 0.196f, 0.941f, 1.0f},
  {0.764f, 1.000f, 0.667f, 1.0f},
  {1.000f, 0.764f, 0.862f, 1.0f},
  {0.294f, 0.098f, 0.902f, 1.0f},
  {0.235f, 0.960f, 0.823f, 1.0f},
  {0.156f, 0.431f, 0.667f, 1.0f},
  {0.502f, 0.502f, 0.000f, 1.0f},
  {0.000f, 0.502f, 0.502f, 1.0f},
};


class Graph : public UIElement
{
public:
  Graph(string name = ""): UIElement(name), auto_resize(true)
  {
    std::srand(time(nullptr));
  }

  void PushData(vector<double>& x, vector<vector<double>>& ys);
  void Draw();

  void SetMaxLenght(int);
  void SetScale(double, double);
  void AutoResize(bool);

  void SetPosition(Box box);

  void SetLabels(vector<string> vec);

  virtual bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
  virtual bool OnMouseMove(MouseMovedEvent& e);

private:
  void CheckLength(vector<double>* to_check);
  void DrawAxes();

  glm::vec4 GenerateColor();

  double FindMaximum();

  // Vector containing x axis values
  vector<double> x;
  // Vector containing y axis values
  vector<vector<double>> ys;

  vector<string> labels;

  int max_length = 1000;

  float scaleX;
  float scaleY;

  bool auto_resize;

  Box position;
  vector<glm::vec4> line_colors;
  vector<bool> show_line;
};
