#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "vehicle.h"

int main(){

  Chimera chimera;

  string out;
  chimera.serialize_to_string(&out);
  cout << out << endl;

  return 0;
}
