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
#define SELECTED_COLOR 4
#define HOVER_COLOR 1

#define HEADER_HEIGHT 10
#define PRINTABLE_HEIGHT 30

enum Style{
  NORMAL=0,
  BOLD=1,
};

class Browse {
public:
  Browse();
  string start(string path = "/home/filippo/Desktop");

private:
  void move(int, int);
  void clear_screen();
  void print(int, int, string);

  void remove_hidden();

  void print_dirs();
  string get_colored(string text, int color, int style=0);

  vector<directory_entry> all_dirs;

  int index;
  vector<int> selected_index;
  bool hide_hidden_files;

  int count;
  int prev_count;

  int width;
  int height;

  int cursor_x;
  int cursor_y;
};

#endif //BROWSE_H
