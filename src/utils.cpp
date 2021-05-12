#include "utils.h"


bool parse_message(string str, message* msg){
  // removing unnecessary chars
  range::remove_erase_if(str, is_any_of("()"));

  vector<string> splitted;
  split(splitted, str, is_any_of(" "));

  if(splitted.size() < 3)
    return false;

  // converting first string (timestamp) to double
  msg->timestamp = stod(splitted[0]);

  split(splitted, splitted.at(2), is_any_of("#"));

  if(splitted.size() < 1)
    return false;

  algorithm::to_lower(splitted[0]);
  msg->id = std::stoi(splitted[0], nullptr, 16);

  msg->size = 0;
  for(uint8_t i = 0; i < splitted[1].size()-2; i+=2){
    string bff = splitted[1].substr(i, 2);
    algorithm::to_lower(bff);
    if(bff.size() <= 1)
      continue;
    msg->data[msg->size] = (uint8_t)(std::stoi(bff, nullptr, 16));
    msg->size ++;
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
