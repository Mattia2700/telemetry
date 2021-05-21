#include <ctime>
#include <chrono>
#include <stdio.h>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <iostream>

#include <boost/filesystem.hpp>

#include <mutex>
#include <thread>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "utils.h"
#include "browse.h"
#include "devices.h"
#include "vehicle.h"

using namespace std;
using namespace std::chrono;
using namespace boost::filesystem;

#define MAX_MESSAGES 20


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

  for (string path : selected_paths){
    auto files  = get_all_files(path, ".log");

    auto candump_files = get_candump_from_files(files);
    if(candump_files.size() <= 0){
      cout << "No candump found... exiting" << endl;
      return -1;
    }

    for (auto file : files){

      string folder;
      try{
        int n = stoi(remove_extension(file));
        folder = get_parent_dir(file) + "/" + to_string(n) + "_checker";
      }catch(std::exception& e){
        folder = get_parent_dir(file) + "/checker";
      }

      Chimera chimera;

      create_directory(folder);

      chimera.add_filenames(folder, ".json");     // to store parsed values
      chimera.add_filenames(folder, ".messages");      // to store raw message
      chimera.open_all_files();

      int devices_to_check = chimera.devices.size() -1;
      vector<int> checked_ids;

      for(auto device : chimera.devices){
        device->count = 0;
      }

      message msg;
      vector<string> lines;
      get_lines(file, &lines);
      Device* modified = nullptr;
      for(int i = 20; i < lines.size(); i++){
        time_point t_start = high_resolution_clock::now();
        if(!parse_message(lines [i], &msg))
          continue;
        modified = chimera.parse_message(msg.timestamp, msg.id, msg.data, msg.size);


        if(modified != nullptr){
          if(modified->count > MAX_MESSAGES){
            std::vector<int>::iterator it;
            it = find (checked_ids.begin(), checked_ids.end(), modified->get_id());

            // If the device was already checked continue
            if (it != checked_ids.end()){
              continue;
              if(checked_ids.size() >= devices_to_check){
                break;
              }
            }else{
              checked_ids.push_back(modified->get_id());
              if(checked_ids.size() >= devices_to_check){
                break;
              }
            }
          }
          else{
            modified->count ++;
          }

          string header = modified->get_header(";");
          vector<string> keys = split(header, ';');
          string data = modified->get_string(";");
          vector<string> values = split(data, ';');

          nlohmann::ordered_json json_values;
          for(int j = 0; j < keys.size(); j++){
            json_values[keys[j]] = values[j];
          }
          *modified->files[0] << json_values << "\n";
          *modified->files[1] << lines[i];
        }
      }
      chimera.close_all_files();
    }
  }

  return 0;
}
