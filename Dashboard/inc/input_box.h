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

#include <functional>

using namespace std;
using namespace cv;

#include "renderer.h"

#define BASE_FONT_SIZE 0.5
#define BASE_FONT_TYPE FONT_HERSHEY_DUPLEX
class Page11;

namespace input
{
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
}

class InputBox : public UIElement
{
public:
  InputBox(string name, Box position_):
  UIElement(name),
  position(position_)
  {
    text.type = input::PARAGRAPH;
    text.color = cv::Scalar(255,255,255,255);
  };

  void SetText(const input::Text& text_);

  void Clear();

  void Draw(Mat* img);

  void CommandMode(char key);

  template<typename A, typename B>
  void SetOnInput(A func_ptr, B obj_ptr)
  {
    m_OnInput = bind(func_ptr, obj_ptr, placeholders::_1, placeholders::_2);
  }

  template<typename A, typename B>
  void SetOnEnter(A func_ptr, B obj_ptr)
  {
    m_OnEnter = bind(func_ptr, obj_ptr, placeholders::_1, placeholders::_2);
  }

protected:
  Box position;

  input::Text text;

private:
  cv::Size GetCharSize(input::TextType type);
  cv::Size GetTextSize(const input::Text& line);


  function<void(UIElement*, string)> m_OnEnter;
  function<void(UIElement*, char)> m_OnInput;
};
