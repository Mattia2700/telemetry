#include <vector>
#include <thread>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <exception>

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
    cout << "\t" << get_colored(sel, 9, 1) << endl;
  cout << endl;

  time_point t_start = high_resolution_clock::now();
  for (string path : selected_paths){
    auto files  = get_all_files(path, ".log");

    auto candump_files = get_candump_from_files(files);
    if(candump_files.size() <= 0){
      cout << "No candump found... exiting" << endl;
      return -1;
    }

    int chunks = thread::hardware_concurrency();
    if(candump_files.size() < chunks)
      chunks = candump_files.size();
    int increment = candump_files.size() / chunks;
    int i0 = 0;
    int i1 = increment;
    for(int i = 0; i < chunks; i++){
      if(i0 > i1 || i1 > candump_files.size())
        continue;

      vector<string>::const_iterator first = candump_files.begin() + i0;
      vector<string>::const_iterator last =  candump_files.begin() + i1;
      vector<string> chunk(first, last);
      if(chunk.size() <= 0)
        continue;
      thread* new_thread = new thread(parse_files, chunk);
      active_threads.push_back(new_thread);
      cout << get_colored("Starting thread: "+ to_string(i) + " of: " + to_string(chunks), 3) << endl;

      i0 += increment;
      i1 += increment;
    }
    cout << endl;
  }
  for(auto t:active_threads)
    t->join();
  double dt = duration<double, milli>(high_resolution_clock::now() - t_start).count()/1000;
  cout << endl;
  cout << get_colored("Total Execution Time: " + to_string(dt) + " seconds", 2) << endl;
  cout << get_colored("Parsed " + to_string(total_lines) + " lines!!", 2) << endl;
  cout << get_colored("Average " + to_string(int(total_lines / dt)) + " lines/sec!!", 2) << endl;

  return 0;
}

void parse_files(vector<string> files){
  for(auto file:files)
    parse_file(file);
}


void parse_file(string fname){

  string folder;
  try{
    int n = stoi(remove_extension(fname));
    folder = get_parent_dir(fname) + "/" + to_string(n);
  }catch(std::exception& e){
    folder = get_parent_dir(fname) + "/parsed";
  }

  Chimera chimera;

  create_directory(folder);

  chimera.set_all_filenames(folder, ".csv");
  chimera.open_all_files();
  chimera.write_all_headers();

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
