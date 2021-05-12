#include "csv.h"


int main(){
  Chimera chimera;

  for(auto device : chimera.devices){
    cout << device->get_id() << " " << device->get_name() << endl;
  }

  string fname = "/media/filippo/label/Codes/Github/telemetry/5.log";

  message msg;
  vector<string> lines;
  get_lines(fname, &lines);

  ofstream out("out.csv");
  double current = 0;
  double increment = 200 / lines.size();
  for(int i = 20; i < lines.size(); i++){
    if(!parse_message(lines [i], &msg))
      continue;

    chimera.parse_message(msg.timestamp, msg.id, msg.data, msg.size);
    out << chimera.encoder_left->get_string(";") + "\n";
    // for(auto device: chimera.devices){
    //   cout << device->get_name() << " " << device->get_string(";\t") << endl;
    // }
    double len = 50;
    current = len/lines.size() * i;
    for(int j = 0; j < len; j++){
      if(j == len-1)
        cout << "|";
      else
      if(j < current)
        cout << "-";
      else
        cout << " ";
    }
    cout << "\r" << flush;
  }
  out.close();
  cout << endl;

  return 0;
}
