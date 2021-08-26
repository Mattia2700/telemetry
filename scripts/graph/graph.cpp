#include "graph.hpp"

#include "matplotlibcpp.h"

int main(){

  Browse b;
  b.set_selection_type(sel_all);
  b.set_max_selections(1);
  auto dirs = b.start();

  if(dirs.size() == 0){
    cout << "No directory selected" << endl;
    return -1;
  }

  dirs = get_all_files(dirs[0], ".csv");

  for(string filename : dirs){
    cout << filename << endl;
    vector<string> lines;
    get_lines(filename, &lines);

    for(string line : lines)
      cout << line << endl;
  }

}