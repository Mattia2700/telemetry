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

  // User select a folder
  Browse b;
  b.set_max_selections(1);
  b.set_extension(".log");
  b.set_selection_type(SelectionType::sel_all);
  auto selected_paths = b.start();

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

void parse_file(string fname)
{

  string folder;
  // If the filename has an incremental name create a folder with that number
  // Otherwise create parsed folder
  try
  {
    int n = stoi(remove_extension(fname));
    folder = get_parent_dir(fname) + "/" + to_string(n);
  }
  catch (std::exception &e)
  {
    folder = get_parent_dir(fname) + "/parsed";
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
  for (uint32_t i = 20; i < lines.size(); i++)
  {
    // Try parsing the line
    if (!parse_message(lines[i], &msg))
      continue;
    // Fill the devices
    modifiedDevices = chimera.parse_message(msg.timestamp, msg.id, msg.data, msg.size);

    // For each device modified write the values in the csv file
    for (auto modified : modifiedDevices)
    {
      *modified->files[0] << modified->get_string(";") + "\n";
    }
  }
  // Debug
  double dt = duration<double, milli>(high_resolution_clock::now() - t_start).count() / 1000;
  cout << "Parsed " << lines.size() << " lines in: " << to_string(dt) << " -> " << lines.size() / dt << " lines/sec" << endl;
  chimera.close_all_files();

  // Increment total lines
  total_lines += lines.size();
}
