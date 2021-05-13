#include "utils.h"


bool parse_message(string str, message* msg){
  size_t pos = str.find("can0");
  if (pos != string::npos)
  {
      str.erase(pos-1, 4);
  }
  string bff = "";
  bool timestamp = false;
  bool id = false;
  msg->size = 0;
  for(int i = 1; i < str.size(); i++){
    if(str[i] == ')'){
      msg->timestamp = stod(bff);
      bff = "";
      timestamp = true;
      continue;
    }
    if(str[i] == ' ' || str[i] == '\t' || str[i] == '\n'){
      continue;
    }
    if(str[i] == '#'){
      msg->id = stoi(bff, nullptr, 16);
      bff = "";
      id = true;
      continue;
    }
    bff += str[i];
    if(timestamp && id){
      if(bff.size() == 2){
        msg->data[msg->size] = (uint8_t)(std::stoi(bff, nullptr, 16));
        msg->size ++;
        bff = "";
      }
    }
  }
  return true;
}

void get_lines(string filename, vector<string>* lines){
  FILE* f = fopen(filename.c_str(), "r");

  char* line = NULL;
  size_t size = 0;
  while(getline(&line, &size, f) != -1){
    lines->push_back(line);
  }
  fclose(f);
}

vector<string> get_all_files(string path, string extension){
  std::vector<string> files;
  bool use_extension = extension != "*";

  if (!exists(path) || !is_directory(path))
    throw runtime_error("Directory non existent!");

  for (auto const & entry : recursive_directory_iterator(path))
  {
    if(is_regular_file(entry)){
      if(!use_extension)
        files.push_back(entry.path().string());
      if(use_extension && entry.path().extension() == extension)
        files.push_back(entry.path().string());
    }
  }

  return files;
}

vector<string> get_candump_from_files(vector<string> files){
  vector<string> new_vec;
  for(int i = 0; i < files.size(); i++){
    size_t pos = files.at(i).find("_GPS.log");
    if(pos == string::npos){
      new_vec.push_back(files.at(i));
    }
  }
  return new_vec;
}

string get_parent_dir(string path){
  return boost::filesystem::path(path).parent_path().string();
}

string remove_extension(string path){
  path = boost::filesystem::path(path).filename().string();
  size_t lastindex = path.find_last_of(".");
  if(lastindex != string::npos)
    return path.substr(0, lastindex);
  return "";
}

void mkdir(string path){
  boost::filesystem::path p = path;
  if(!exists(p))
    create_directory(p);
}

bool path_exists(string path){
  return exists(boost::filesystem::path(path));
}

string get_colored(string text, int color, int style){
  return  "\e[" + to_string(style) + ";3" + to_string(color) + "m" + text + "\e[0m";
}
