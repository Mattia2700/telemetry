#include "utils.h"

bool parse_message(string str, message *msg)
{
  size_t pos = str.find("can0");
  if (pos != string::npos)
  {
    str.erase(pos - 1, 4);
  }
  string bff = "";
  bool timestamp = false;
  bool id = false;
  msg->size = 0;
  for (int i = 1; i < str.size(); i++)
  {
    if (str[i] == ')')
    {
      msg->timestamp = stod(bff);
      bff = "";
      timestamp = true;
      continue;
    }
    if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
    {
      continue;
    }
    if (str[i] == '#')
    {
      msg->id = stoi(bff, nullptr, 16);
      bff = "";
      id = true;
      continue;
    }
    bff += str[i];
    if (timestamp && id)
    {
      if (bff.size() == 2)
      {
        msg->data[msg->size] = (uint8_t)(std::stoi(bff, nullptr, 16));
        msg->size++;
        bff = "";
      }
    }
  }
  return true;
}

bool parse_gps_line(string str, gps_message *msg)
{
  size_t pos = str.find("(");
  if (pos == string::npos)
    return false;
  string bff = "";
  bool timestamp = false;
  bool id = false;

  for (int i = pos + 1; i < str.size(); i++)
  {
    if (str[i] == ')')
    {
      msg->timestamp = stod(bff);
      bff = "";
      timestamp = true;
      break;
    }
    bff += str[i];
  }
  pos = str.find("$");
  if (pos == string::npos)
    return false;
  msg->message = str.substr(pos, str.size() - pos);
  return true;
}

void get_lines(string filename, vector<string> *lines)
{
  FILE *f = fopen(filename.c_str(), "r");

  char *line = NULL;
  size_t size = 0;
  lines->clear();
  while (getline(&line, &size, f) != -1)
  {
    lines->push_back(line);
  }
  fclose(f);
}

vector<string> get_all_files(string path, string extension)
{
  std::vector<string> files;
  bool use_extension = extension != "*";

  if (!exists(path) || !is_directory(path))
    throw runtime_error("Directory non existent!");

  for (auto const &entry : recursive_directory_iterator(path))
  {
    if (is_regular_file(entry))
    {
      if (!use_extension)
        files.push_back(entry.path().string());
      if (use_extension && entry.path().extension() == extension)
        files.push_back(entry.path().string());
    }
  }

  return files;
}

vector<string> get_gps_from_files(vector<string> files)
{
  return get_files_with_word(files, "gps");
}
vector<string> get_candump_from_files(vector<string> files)
{
  return get_files_with_word(files, "dump.log");
}
vector<string> get_files_with_word(vector<string> files, string word)
{
  vector<string> new_vec;
  for (int i = 0; i < files.size(); i++)
  {
    size_t pos = files[i].find(word);
    if (pos != string::npos)
    {
      new_vec.push_back(files.at(i));
    }
  }
  return new_vec;
}

string get_parent_dir(string path)
{
  return std::filesystem::path(path).parent_path().string();
}

string remove_extension(string path)
{
  path = std::filesystem::path(path).filename().string();
  size_t lastindex = path.find_last_of(".");
  if (lastindex != string::npos)
    return path.substr(0, lastindex);
  return "";
}

void mkdir(string path)
{
  std::filesystem::path p = path;
  if (!exists(p))
    create_directory(p);
}

bool path_exists(string path)
{
  return exists(std::filesystem::path(path));
}

string get_colored(string text, int color, int style)
{
  return "\e[" + to_string(style) + ";3" + to_string(color) + "m" + text + "\e[0m";
}

vector<string> split(string str, char separator)
{
  vector<string> ret;
  string bff = "";
  for (int i = 0; i < str.size(); i++)
  {
    if (str[i] == separator)
    {
      ret.push_back(bff);
      bff = "";
    }
    else
    {
      bff += str[i];
    }
  }
  ret.push_back(bff);
  return ret;
}

int empty_fields(const vector<string> &vec)
{
  for (int i = 0; i < vec.size(); i++)
    if (vec[i] == "")
      return i;
  return -1;
}

int empty_fields(const vector<string> &vec, const vector<int> &indeces)
{
  for (int i : indeces)
  {
    if (i >= 0 && i < vec.size())
    {
      if (vec[i] == "")
        return i;
    }
  }
  return -1;
}

uint64_t get_timestamp_u()
{
  return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
}
double get_timestamp()
{
  return duration_cast<duration<double, milli>>(system_clock::now().time_since_epoch()).count() / 1000;
}
string get_hex(int num, int zeros)
{
  stringstream ss;
  ss << setw(zeros) << uppercase << setfill('0') << hex << num;
  return ss.str();
}
void get_hex(char *buff, const int &num, const int &zeros)
{
  sprintf(buff, ("%0" + to_string(zeros) + "X").c_str(), num);
}