#pragma once
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

typedef enum Found
{
  FOUND_NONE,
  FOUND_POSSIBLE_START,
  FOUND_START,
  FOUND_POSSIBLE_END,
  FOUND_END
}Found;

using namespace std;

/**
* Parse the given file
*
* @param filename of the file to be parsed
*/
void parse_file(string fname);
