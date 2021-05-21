#include "browse.h"

Browse::Browse(){
  get_winsize();

  index = 0;
  hide_hidden_files = true;

  selection_type = SelectionType::sel_file;

  cursor_x = 0;
  cursor_y = 0;

  stat_fname = string(getenv("HOME")) + "/" + stat_fname;
  if(exists(boost::filesystem::path(stat_fname))){
    std::ifstream stat(stat_fname);
    stat.seekg (0, stat.end);
    int length = stat.tellg();
    stat.seekg (0, stat.beg);
    char * buffer = new char [length];
    stat.read (buffer,length);
    stat.close();
    if(exists(boost::filesystem::path(string(buffer))) &&
    is_directory(boost::filesystem::path(string(buffer))))
      start_path = string(buffer);
    else
      start_path = getenv("HOME");
  }
  else{
    start_path = getenv("HOME");
  }
}

void Browse::set_start_path(string path){
  this->start_path = path;
}
void Browse::set_extension(string extension){
  this->extension = extension;
}
void Browse::set_selection_type(SelectionType type){
  this->selection_type = type;
}
void Browse::set_max_selections(int num){
  this->max_selections = num;
}

vector<string> Browse::start(){
  path = start_path;
  char command;

  char prev_commands[2];
  bool jump_clear = false;

  system("stty raw");

  while(true){
    get_winsize();

    update_dirs();

    if(!jump_clear)
      clear_screen();


    print_dirs(0);
    if(is_directory(all_dirs[index])){
      int prev_idx = index;
      string prev_path = path;
      path = all_dirs[index].path().string();
      index = -1;
      update_dirs();
      print_dirs(1);
      index = prev_idx;
      path = prev_path;
      update_dirs();
    }else{
      print_file(all_dirs[index].path().string(), 1);
    }

    jump_clear = false;

    move(0, height -1);
    cout << get_colored(hint, HINT_COLOR) << flush;
    hint = "";

    command = getchar();
    cout << "\r    \r" << flush;

    if(command == 'q'){
      selected_paths.clear();
      break;
    }
    if(command == 13){
      break;
    }

    // Remap if are arrows
    // arrow codes are 27 then 91 then 65~68
    if(prev_commands[0] == 27  && prev_commands[1] == 91){
      if(command >= 65 && command <= 68){
        if(command == 65)
          command = 'w';
        if(command == 66)
          command = 's';
        if(command == 67)
          command = 'd';
        if(command == 68)
          command = 'a';
      }
    }

    switch (command) {
      case 3:   //CTRL-C
        cout << "use q to quit" << endl;
      break;
      case 'a':
        if(path != "/"){
          string prev_path = path;

          path = boost::filesystem::path(path).parent_path().string();
          update_dirs();

          vector<directory_entry>::iterator pos;
          pos = find(all_dirs.begin(), all_dirs.end(), directory_entry(prev_path));
          if(pos != all_dirs.end()){
            index = pos - all_dirs.begin();
          }
          else{
            index = 0;
          }
        }
        else{
          hint = "You are in root, can't go back";
        }
      break;
      case 'd':
        if(is_directory(all_dirs[index].path())){
          path = all_dirs[index].path().string();
          index = 0;
        }
        else{
          hint = "It is a file. Use space to select";
        }
      break;
      case 's':
        index = (index + 1) % count;
      break;
      case 'w':
        index = (index - 1) % count;
        if(index < 0)
          index = count-1;
      break;
      case ' ':
        select();
      break;
      case 'h':
        if(hide_hidden_files == false)
          hide_hidden_files = true;
        else
          hide_hidden_files = false;
      break;
    }

    prev_commands[0] = prev_commands[1];
    prev_commands[1] = command;
  }
  clear_screen();
  system("stty cooked");

  if(selected_paths.size() > 0){
    std::ofstream stat(stat_fname);
    stat << boost::filesystem::path(selected_paths.at(0)).parent_path().string();;
    stat.close();
  }

  return selected_paths;
}

void Browse::clear_screen(){
  move(0,0);
  for(int i = 0; i < height; i++)
    cout << "\033[K" << "\r\n";
  move(0,0);
}

void Browse::move(int x, int y){
  cout << "\033[" << y << ";" << x << "H  " << flush;
}

void Browse::print(int x, int y, string text){
  move(x, y);
  cout << text << flush;
}

void Browse::print_dirs(int column){
  int x = column * (COLUMN_MAX_WIDTH+2);
  if(x > width)
    return;

  move(0, HEADER_HEIGHT);

  int color = DIR_COLOR;
  int style = NORMAL;

  string prepend = "";

  int scroll = index - PRINTABLE_HEIGHT/2;
  if(scroll < 0)
    scroll = 0;

  for(int i = scroll; i < all_dirs.size() && i < PRINTABLE_HEIGHT + scroll; i++){
    color = DIR_COLOR;
    style = NORMAL;
    prepend = "";

    string fname = all_dirs[i].path().filename().string();

    if(is_regular_file(all_dirs[i].path()))
      color = FILE_COLOR;

    if(i == index){
      color = HOVER_COLOR;
      style = BOLD;

      prepend = "-> ";
    }
    if(is_selected(all_dirs[i].path().string())){
      color = SELECTED_COLOR;
      style = BOLD;
      prepend = "--> ";
    }
    fname = prepend + fname;

    if(x + fname.size() > width-1){
      fname.erase(fname.begin() + width - x-1, fname.end());
    }
    if(fname.size() > COLUMN_MAX_WIDTH){
      fname.erase(fname.end()+COLUMN_MAX_WIDTH-fname.size(), fname.end());
    }

    move(x, HEADER_HEIGHT + i - scroll);
    cout << get_colored(fname, color, style);
  }
  cout << flush;
  move(0, HEADER_HEIGHT + index - scroll);
}

string Browse::get_colored(string text, int color, int style){
  return  "\e[" + to_string(style) + ";3" + to_string(color) + "m" + text + "\e[0m";
}

void Browse::remove_hidden(){
  int removed = 0;
  for(int i = all_dirs.size()-1; i > 0 ; i--){
    if(all_dirs[i].path().filename().string()[0] == '.'){
      all_dirs.erase(all_dirs.begin() + i);
      removed ++;
    }
  }
}

void Browse::select(){
  if((selection_type == sel_folder || selection_type == sel_all) && is_directory(all_dirs[index].path())){
    int idx = get_selected_index(all_dirs[index].path().string());
    if(idx == -1)
      selected_paths.push_back(all_dirs[index].path().string());
    else
      selected_paths.erase(selected_paths.begin() + idx);
  }
  else if((selection_type == sel_file || selection_type == sel_all) && is_regular_file(all_dirs[index])){
    if(extension == "*" || all_dirs[index].path().extension() == extension){
      int idx = get_selected_index(all_dirs[index].path().string());
      if(idx == -1)
        selected_paths.push_back(all_dirs[index].path().string());
      else
        selected_paths.erase(selected_paths.begin() + idx);
    }else{
      hint = "The file does not match extension: " + extension;
    }
  }
  while(selection_type != -1 && selected_paths.size() >= selection_type)
    selected_paths.erase(selected_paths.begin());
}

bool Browse::is_selected(string path){
  vector<string>::iterator pos;
  pos = find(selected_paths.begin(), selected_paths.end(), path);
  if(pos != selected_paths.end())
    return true;
  return false;
}

int Browse::get_selected_index(string path){
  vector<string>::iterator pos;
  pos = find(selected_paths.begin(), selected_paths.end(), path);
  if(pos != selected_paths.end())
    return pos - selected_paths.begin();
  return -1;
}

void Browse::update_dirs(){
    all_dirs.clear();
    copy(directory_iterator(path), directory_iterator(), back_inserter(all_dirs));
    if(hide_hidden_files){
      remove_hidden();
    }
    sort(all_dirs.begin(), all_dirs.end());
    count = all_dirs.size();
}

void Browse::get_winsize(){
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  width = w.ws_col;
  height = w.ws_row;
}

void Browse::print_file(string fname, int column){
  if(!boost::filesystem::is_regular_file(fname))
    return;
  if(!exists(fname))
    return;


  FILE* f = fopen(fname.c_str(), "r");
  char* line = NULL;
  size_t size = 0;
  int x = column * (COLUMN_MAX_WIDTH + 2);
  int count = 0;
  while(getline(&line, &size, f) != -1){
    if(count > PRINTABLE_HEIGHT)
      break;
    for(int i = 0; i< size; i++){
      if(line[i] == '\r')
        line[i] = ' ';
    }

    string line_str = string(line);
    if(line_str.size() > COLUMN_MAX_WIDTH){
      line_str.resize(COLUMN_MAX_WIDTH);
    }

    move(x, HEADER_HEIGHT + count);
    cout << line_str;
    count ++;
  }

  fclose(f);
}
