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

  for(uint8_t i = 0; i < splitted[1].size()-2; i+=2){
    string bff = splitted[1].substr(i, 2);
    algorithm::to_lower(bff);
    if(bff.size() <= 1)
      continue;
    msg->data[i] = std::stoi(bff, nullptr, 16);
  }

  return true;
}

vector<string> get_lines(string filename){
  std::vector<string>* lines = new vector<string>();
  ifstream f(filename);

  string line;
  while(getline(f, line)){
    std::istringstream iss(line);
    int a, b;
    if (!(iss >> a >> b)) { break; } // error
    lines->push_back(line);
  }

}
