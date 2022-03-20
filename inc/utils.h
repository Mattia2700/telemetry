#ifndef UTILS_H
#define UTILS_H

#include <ctime>
#include <chrono>
#include <stdio.h>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>
#include <string.h>
#include <iostream>


#include <filesystem>

using namespace std;
using namespace std::chrono;
using namespace filesystem;


struct message {
  double timestamp;
  int id;
  int size;
  uint8_t data[8];
};
struct gps_message
{
  double timestamp;
  string message;
};

bool parse_message(string str, message* msg);
bool parse_gps_line(string str, gps_message* msg);
void get_lines(string filename, vector<string>* lines);

/**
* Gets current timestamp in seconds
*/
double get_timestamp();

vector<string> get_all_files(string path, string extension="*");

vector<string> get_gps_from_files(vector<string> files);
vector<string> get_files_with_word(vector<string> files, string word);
vector<string> get_candump_from_files(vector<string> files);

string get_parent_dir(string path);

string remove_extension(string path);

void mkdir(string path);

bool path_exists(string path);

string get_colored(string text, int color, int style = 1);

vector<string> split(string str, char separator);

// returns -1 if all fields are filled
// returns index of first empty field
int empty_fields(const vector<string>& vec);

// indeces to be checked
// returns -1 if all fields are filled
// returns index of first empty field
int empty_fields(const vector<string>& vec, const vector<int>& indeces);


/**
* Returns a string whith int expressed as Hexadecimal
* Capital letters
*
* @param num number to be converted
* @param zeros length of the final string (num = 4 => 0000A)
* return string
*/
string get_hex(int num, int zeros);

#endif // UTILS_H
