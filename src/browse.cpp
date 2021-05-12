#include "browse.h"

Browse::Browse(){
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  width = w.ws_col;
  height = w.ws_row;

  index = 0;
  hide_hidden_files = true;

  cursor_x = 0;
  cursor_y = 0;
}

string Browse::start(string path){
  char command;

  char prev_commands[2];
  bool jump_clear = false;
  while(true){
    all_dirs.clear();
    copy(directory_iterator(path), directory_iterator(), back_inserter(all_dirs));
    if(hide_hidden_files){
      remove_hidden();
    }
    count = all_dirs.size();

    if(!jump_clear)
      clear_screen();
    print_dirs();
    move (0, height -1);

    jump_clear = false;

    system("stty raw");
    command = getchar();
    system("stty cooked");
    cout << "\r    \r" << flush;

    if(command == 'q')
      break;

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
        index = 0;
        path = boost::filesystem::path(path).parent_path().string();
      break;
      case 'd':
        index = 0;
        if(is_directory(all_dirs[index].path())){
          path = all_dirs[index].path().string();
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
  return "";
}

void Browse::clear_screen(){
  move(0,0);
  cout << "\033[2J" << flush;
}

void Browse::move(int x, int y){
  cout << "\033[" << y << ";" << x << "H" << flush;
}

void Browse::print(int x, int y, string text){
  move(x, y);
  cout << text << flush;
}

void Browse::print_dirs(){
  move(0, HEADER_HEIGHT);

  int color = DIR_COLOR;
  int style = NORMAL;

  int scroll = index - PRINTABLE_HEIGHT/2;
  if(scroll < 0)
    scroll = 0;

  for(int i = scroll; i < all_dirs.size() && i < PRINTABLE_HEIGHT + scroll; i++){
    color = DIR_COLOR;
    style = NORMAL;

    string fname = all_dirs[i].path().filename().string();

    if(is_regular_file(all_dirs[i].path()))
      color = 3;

    if(i == index){
      color = SELECTED_COLOR;
      style = BOLD;

      fname = "-> " + fname;
    }

    cout << get_colored(fname, color, style) << endl;
  }
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
