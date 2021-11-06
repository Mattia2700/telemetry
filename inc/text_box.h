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

#define BASE_FONT_SIZE 0.5
#define BASE_FONT_TYPE FONT_HERSHEY_DUPLEX

enum TextType
{
  TITLE,
  SUBTITLE,
  PARAGRAPH,
  TEXT_TYPE_SIZE
};

static float FONT_SIZES[TEXT_TYPE_SIZE] =
{
  BASE_FONT_SIZE * 1.4,
  BASE_FONT_SIZE * 1.1,
  BASE_FONT_SIZE * 1.0
};

static float CHAR_SIZE;

struct Text
{
  TextType type;
  string content;
  cv::Scalar color;
};


class TextBox : public UIElement
{
public:
  TextBox(string name, Box position_):
  UIElement(name),
  position(position_)
  {
  }

  void SetLines(const vector<Text>& text_);
  void AppendLine(const Text& line_);

  void Clear();

  void Draw(Mat* img);

  void CommandMode(char key){};

protected:
  Box position;

  vector<Text> text;

private:
  void WrapLine(Text& line);
  cv::Size GetCharSize(TextType type);
  cv::Size GetTextSize(const Text& line);
};
