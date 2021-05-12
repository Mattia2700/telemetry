#include "csv.h"

#include <vector>
#include <thread>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "browse.h"
#include "vehicle.h"

using namespace std;

long long int total_lines = 0;
void parse_file(string fname);
void parse_files(vector<string> files);

vector<thread* > active_threads;

int main(){

  Browse b;
  b.set_max_selections(1);
  b.set_extension(".log");
  b.set_selection_type(SelectionType::sel_all);
  auto selected_paths = b.start();

  if(selected_paths.size() <= 0){
    cout << "No file selected... exiting" << endl;
    return -1;
  }

  cout << "Selected paths: " << endl;
  for(auto sel : selected_paths)
    cout << sel << endl;

  time_point t_start = high_resolution_clock::now();
  for (string path : selected_paths){
    auto files  = get_all_files(path, ".log");

    auto candump_files = get_candump_from_files(files);
    if(candump_files.size() <= 0){
      cout << "No candump found... exiting" << endl;
      return -1;
    }

    cout << thread::hardware_concurrency() << endl;

    int chunks = thread::hardware_concurrency();
    int increment = candump_files.size() / chunks;
    int i0;
    int i1 = increment;
    for(int i = 0; i < chunks; i++){
      vector<string>::const_iterator first = candump_files.begin() + i0;
      vector<string>::const_iterator last =  candump_files.begin() + i1;
      vector<string> chunk(first, last);

      thread* new_thread = new thread(parse_files, chunk);
      active_threads.push_back(new_thread);
      cout << "Starting thread: "<< i<< " of: " << chunks<<endl;

      i0 += increment;
      i1 += increment;
    }


    // for(auto candump : candump_files){
    //   parse_file(candump);
    // }
  }
  for(auto t:active_threads)
    t->join();
  double dt = duration<double, milli>(high_resolution_clock::now() - t_start).count()/1000;
  cout << "Total Execution Time: " << dt << " seconds" << endl;
  cout << "Parsed " << total_lines << " lines!!" << endl;

  return 0;
}

void parse_files(vector<string> files){
  for(auto file:files)
    parse_file(file);
}


void parse_file(string fname){
  Chimera chimera;

  string folder = get_parent_dir(fname) + "/parsed";

  create_directory(folder);

  chimera.set_all_filenames(folder, ".csv");
  chimera.open_all_files();

  message msg;
  vector<string> lines;
  get_lines(fname, &lines);

  double current = 0;

  Device* modified = nullptr;
  time_point t_start = high_resolution_clock::now();
  for(int i = 20; i < lines.size(); i++){
    time_point t_start = high_resolution_clock::now();
    if(!parse_message(lines [i], &msg))
      continue;
    modified = chimera.parse_message(msg.timestamp, msg.id, msg.data, msg.size);

    if(modified != nullptr){
      *modified->file << modified->get_string(";") + "\n";
    }
  }
  double dt = duration<double, milli>(high_resolution_clock::now() - t_start).count()/1000;
  cout << "Parsed " << lines.size() << " lines in: " << to_string(dt) << " -> " << lines.size()/dt << " lines/sec" << endl;
  chimera.close_all_files();

  total_lines += lines.size();
}
