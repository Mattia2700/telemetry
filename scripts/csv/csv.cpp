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

#include "report.h"

#include "can_stat_json/json_loader.h"
#include "gps_stat_json/json_loader.h"
#include "csv_parser_config/json_loader.h"

using namespace std;

csv_parser_config config;
/**
* Total lines parse
*/
long long int total_lines = 0;
/**
* Parse the given file
*
* @param filename of the file to be parsed
*/
void parse_file(string fname);
/**
* Parses all the files in the vector
*
* @param files vector of filenames
*/
void parse_files(vector<string> files);

// Add here a thread when is started
vector<thread *> active_threads;

int main()
{

  string home = getenv("HOME");
  string config_path = home + "/csv_parser_config.json";
  if(fs::exists(config_path))
  {
    LoadJson(config, config_path);
  }
  else
  {
    config.subfolder_name = "Parsed";
    config.parse_gps = true;
    config.parse_candump = true;
    config.generate_report = true;
    SaveJson(config, config_path);
  }

  // User select a folder
  Browse b;
  b.SetMaxSelections(1);
  b.SetExtension("*");
  b.SetSelectionType(SelectionType::sel_folder);
  auto selected_paths = b.Start();

  if (selected_paths.size() <= 0)
  {
    cout << "No file selected... exiting" << endl;
    return -1;
  }

  cout << "Selected paths: " << endl;
  for (auto sel : selected_paths)
    cout << "\t" << get_colored(sel, 9, 1) << endl;
  cout << endl;

  // Start the timer
  time_point t_start = high_resolution_clock::now();
  for (string path : selected_paths)
  {
    // Get all the files with that extension
    auto files = get_all_files(path, ".log");

    // Select only candump files
    auto candump_files = get_candump_from_files(files);
    if (candump_files.size() <= 0)
    {
      cout << "No candump found... exiting" << endl;
      return -1;
    }

    // Divide all files for number of cores available.
    int chunks = thread::hardware_concurrency();
    // chunks = 1;
    if (candump_files.size() < chunks)
      chunks = candump_files.size();
    int increment = candump_files.size() / chunks;
    int i0 = 0;
    int i1 = increment;
    for (int i = 0; i < chunks; i++)
    {
      if (i0 > i1 || i1 > candump_files.size())
        continue;

      // Divide in chunks
      vector<string>::const_iterator first = candump_files.begin() + i0;
      vector<string>::const_iterator last = candump_files.begin() + i1;
      vector<string> chunk(first, last);
      if (chunk.size() <= 0)
        continue;

      // Assign to every thread the chunk
      thread *new_thread = new thread(parse_files, chunk);
      active_threads.push_back(new_thread);
      cout << get_colored("Starting thread: " + to_string(i) + " of: " + to_string(chunks), 3) << endl;

      i0 += increment;
      i1 += increment;
    }
    cout << endl;
  }
  // Wait all the threads
  for (auto t : active_threads)
    t->join();

  // Debug
  double dt = duration<double, milli>(high_resolution_clock::now() - t_start).count() / 1000;
  cout << endl;
  cout << get_colored("Total Execution Time: " + to_string(dt) + " seconds", 2) << endl;
  cout << get_colored("Parsed " + to_string(total_lines) + " lines!!", 2) << endl;
  cout << get_colored("Average " + to_string(int(total_lines / dt)) + " lines/sec!!", 2) << endl;

  return 0;
}

void parse_files(vector<string> files)
{
  for (auto file : files)
    parse_file(file);
}

// parse a candump file and a related gps file (if exists)
void parse_file(string fname)
{
  int lines_count = 0;
  Report report;

  string folder = get_parent_dir(fname);

  auto files = get_all_files(folder, ".log");
  auto gps_files = get_gps_from_files(files);

  can_stat_json can_stat;
  auto json_files = get_all_files(folder, ".json");
  auto can_stat_files = get_files_with_word(json_files, "CAN_Info");

  if(can_stat_files.size() != 0)
  {
    try
    {
      LoadJson(can_stat, can_stat_files[0]);
    }
    catch(exception e)
    {
      cout << "Exception loading json " << e.what() << endl;
    }
  }

  // If the filename has an incremental name create a folder with that number
  // Otherwise create parsed folders
  try
  {
    int n = stoi(remove_extension(fname));
    folder += "/" + to_string(n);
  }
  catch (std::exception &e)
  {
    folder += "/" + config.subfolder_name;
  }
  
  create_directory(folder);

  Chimera chimera;

  // Add csv files for each device
  // Open them
  // Write CSV header (column name)
  chimera.add_filenames(folder, ".csv");
  chimera.open_all_files();
  chimera.write_all_headers(0);

  // Get all lines
  message msg;
  vector<string> lines;
  get_lines(fname, &lines);

  // Contains devices modified from the CAN message
  vector<Device *> modifiedDevices;

  // Start Timer
  time_point t_start = high_resolution_clock::now();
  double prev_timestsamp;
  if(config.parse_candump)
  {
    for (uint32_t i = 20; i < lines.size(); i++)
    {
      // Try parsing the line
      if (!parse_message(lines[i], &msg))
        continue;
      // Fill the devices
      modifiedDevices = chimera.parse_message(msg.timestamp, msg.id, msg.data, msg.size);

      if (prev_timestsamp > msg.timestamp)
      {
        cout << fname << "\n";
        cout << std::fixed << setprecision(9) << msg.timestamp << "\t";
        cout << lines[i] << "\t" << i << endl;
      }

      // For each device modified write the values in the csv file
      for (auto modified : modifiedDevices)
      {
        *modified->files[0] << modified->get_string(",") + "\n";
        report.AddDeviceSample(&chimera, modified);
      }
      prev_timestsamp = msg.timestamp;
    }
  }
  lines_count += lines.size();

  if(config.parse_gps)
  {
    for(auto gps_file : gps_files)
    {
      Gps* current_gps;
      
      if(fs::path(gps_file).filename().string().find("2") != string::npos)
        current_gps = chimera.gps2;
      else
        current_gps = chimera.gps1;

      get_lines(gps_file, &lines);
      gps_message msg;
      for(size_t i = 20; i < lines.size(); i++)
      {
        if(!parse_gps_line(lines[i], &msg))
          continue;

        int ret = chimera.parse_gps(current_gps, msg.timestamp, msg.message);
        if (ret == 1)
        {
          *current_gps->files[0] << current_gps->get_string(",") + "\n";
        }
      }
    }
  }
  lines_count += lines.size();


  if(config.generate_report)
  {
    string header = "";
    if(can_stat.Date != "" && can_stat.Pilot != "")
    {

    }
    report.Clean(1920*2);
    report.Generate(folder + "/Report.pdf");
  }
  // Debug
  double dt = duration<double, milli>(high_resolution_clock::now() - t_start).count() / 1000;
  cout << "Parsed " << lines_count << " lines in: " << to_string(dt) << " -> " << lines_count / dt << " lines/sec" << endl;
  chimera.close_all_files();

  // Increment total lines
  total_lines += lines_count;
}
