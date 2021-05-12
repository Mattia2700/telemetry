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

#define DIR_COLOR 3
#define FILE_COLOR 9
#define SELECTED_COLOR 1
#define HOVER_COLOR 4
#define HINT_COLOR 6

#define HEADER_HEIGHT 10
#define PRINTABLE_HEIGHT 30

enum SelectionType{
  sel_file,
  sel_folder,
  sel_all,
};

enum Style{
  NORMAL=0,
  BOLD=1,
};

class Browse {
public:
  Browse();
  vector<string> start(string path = "/home/filippo/Desktop");

  void set_max_selections(int num=-1);
  void set_extension(string extension);
  void set_selection_type(SelectionType type);


private:
  void move(int, int);
  void clear_screen();
  void print(int, int, string);

  void select();
  bool is_selected(string path);
  int get_selected_index(string path);

  void remove_hidden();

  void print_dirs();
  string get_colored(string text, int color, int style=0);

  vector<directory_entry> all_dirs;

  int index;
  vector<string> selected_paths;
  bool hide_hidden_files;

  int count;
  int prev_count;

  int width;
  int height;

  int cursor_x;
  int cursor_y;

  string hint="";
  string extension="*";
  int max_selections=-1;
  SelectionType selection_type;
};

#endif //BROWSE_H
