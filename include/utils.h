#ifndef UTILS_H
#define UTILS_H

#include <ctime>
#include <chrono>
#include <stdio.h>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>
#include <string.h>
#include <iostream>


#include <boost/range/algorithm/remove_if.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace std::chrono;


struct message {
  double timestamp;
  int id;
  int size;
  uint8_t data[8];
};

bool parse_message(string str, message* msg);

vector<string> get_lines(string filename);

#endif // UTILS_H
