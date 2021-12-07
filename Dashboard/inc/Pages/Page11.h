#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>

#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "devices.pb.h"

#include "renderer.h"
#include "input_box.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
using namespace rapidjson;

#include "common_definitions.h"

using namespace std;
using namespace cv;


class Page11 : public Page
{
public:
  Page11(string name, int w, int h);

  virtual int Draw();

  template<typename A, typename B>
  void SetOnEnter(A func_ptr, B obj_ptr)
  {
    m_OnEnter = bind(func_ptr, obj_ptr, placeholders::_1);
  }
  void SetOnEnter(function<void(string)> func)
  {
    m_OnEnter = func;
  }
private:

  void SetTextData();

  void on_enter(UIElement*, string);
  void on_input(UIElement*, char);

  void load_config(string& path);
  void write_config(string& path);


  InputBox* pilot_ib;
  InputBox* circuit_ib;
  InputBox* race_ib;

  string m_Config_FName;

  run_config config;

  function<void(string)> m_OnEnter;
};
