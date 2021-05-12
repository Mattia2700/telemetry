#ifndef BROWSE_H
#define BROWSE_H

#include <sys/ioctl.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;

#define DIR_COLOR 9
#define FILE_COLOR 2
#define SELECTED_COLOR 1
#define HOVER_COLOR 1

#define HEADER_HEIGHT 10

class Browse {
public:
  Browse();
  string start(string path = "/");

private:
  void move(int, int);
  void clear_screen();
  void print(int, int, string);

  void print_dirs();
  string get_colored(string text, int color);

  vector<directory_entry> all_dirs;
  int index;

  int count;
  int prev_count;

  int width;
  int height;

  int cursor_x;
  int cursor_y;
};

#endif //BROWSE_H
