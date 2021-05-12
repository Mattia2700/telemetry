#include "browse.h"

Browse::Browse(){
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  width = w.ws_col;
  height = w.ws_row;

  cursor_y = 0;
}

string Browse::start(string path){
  char command;

  char prev_commands[2];
  while(true){
    all_dirs.clear();
    copy(directory_iterator(path), directory_iterator(), back_inserter(all_dirs));

    // clear_screen();
    // print_dirs();

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
        cout << "back"<< endl;
      break;
      case 'd':
        cout << "forward"<< endl;
      break;
      case 's':
        cout << "down" << endl;
      break;
      case 'w':
        cout << "up" << endl;
      break;
    }

    prev_commands[0] = prev_commands[1];
    prev_commands[1] = command;
  }
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
  for(int i = 0; i < all_dirs.size(); i++){
    string fname = all_dirs[i].path().filename().string();
    int color = DIR_COLOR;
    if(is_regular_file(all_dirs[i]))
      color = FILE_COLOR;

    cout << get_colored(fname, color) << endl;
  }
  move(0, HEADER_HEIGHT);
}

string Browse::get_colored(string text, int color){
  return  "\e[1;1" + to_string(color) + "m" + text + "\e[0m";
}
