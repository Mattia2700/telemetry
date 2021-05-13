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


#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/algorithm/remove_if.hpp>

using namespace std;
using namespace boost;
using namespace std::chrono;
using namespace boost::filesystem;


struct message {
  double timestamp;
  int id;
  int size;
  uint8_t data[8];
};

bool parse_message(string str, message* msg);

void get_lines(string filename, vector<string>* lines);

vector<string> get_all_files(string path, string extension="*");

vector<string> get_candump_from_files(vector<string> files);

string get_parent_dir(string path);

string remove_extension(string path);

void mkdir(string path);

bool path_exists(string path);

string get_colored(string text, int color, int style = 1);

#endif // UTILS_H
