#pragma once

#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <unordered_map>

#include <filesystem>

#include "vehicle.h"
#include "devices.h"
#include "matplotlibcpp.h"

#include "libharu/include/hpdf.h"

#include "gnuplot-iostream.h"

using namespace std;
using namespace gnuplotio;
namespace plt = matplotlibcpp;
namespace fs = std::filesystem;


void error_handler  (HPDF_STATUS   error_no,
                HPDF_STATUS   detail_no,
                void         *user_data);
void
show_description (HPDF_Page    page,
                  float        x,
                  float        y,
                  const char  *text);


template <class T1>
vector<T1> upsample(const vector<T1> &x, const int p)
{
  long int N = x.size();
  vector<T1> y;
  y.reserve(p * N);
  for (long int n = 0; n < N; n++)
    y[p * n] = x[n];
  return y;
}

template <class T1>
vector<T1> downsample(const vector<T1> &x, const int q)
{
  vector<T1> y;
  int N = int(floor(1.0 * x.size() / q));
  y.reserve(N);
  for (long int n = 0; n < N; n++)
    y[n] = x[n * q];
  return y;
}


struct MapElement
{
  string primary;
  string secondary;
};

struct CurrentFont
{
  string font_name;
  HPDF_Font font;
  int size;
};

class Report
{
public:
  Report(){};

  void AddDeviceSample(Chimera* chim, Device* device);
  void Generate(string path);
  void Clean(int);
  void Filter(const vector<double> &in, vector<double>* out, int window=10);
  void DownSample(const vector<double>& in, vector<double>* out, int count);

private:
  unordered_map<string, unordered_map<string, vector<double>>> sensor_data;

  unordered_map<string, CurrentFont> c_fonts;

private:
};