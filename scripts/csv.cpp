#include "csv.h"

int main(){

  Browse b;
  b.set_max_selections(1);
  b.set_extension(".log");
  b.set_selection_type(SelectionType::sel_all);
  auto b_res = b.start();
  cout << "Selected paths: " << endl;
  for(auto sel : b_res)
    cout << sel << endl;

  

  return 1;

  string folder = "/home/filippo/Downloads/Telegram Desktop/CANDUMP_DEFAULT_FOLDER/16-dic-2020__11-55-43";

  auto files  = get_all_files("/home/filippo/Downloads/Telegram Desktop/CANDUMP_DEFAULT_FOLDER", ".log");
  // for(string file : files)
  //   cout << file << endl;

  auto candump_files = get_candump_from_files(files);
    // for (string candump : candump_files)
    //   cout << candump << endl;

  Chimera chimera;

  chimera.set_all_filenames(folder + "/0", ".csv");
  chimera.open_all_files();

  string fname = folder + "/0.log";

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

  return 0;
}
