#include "dashboard.h"

int main(){

  /*if(setup_curl("http://127.0.0.1:8000/Dashboard/realTime/setData") < 0){
    cout << "FAILED SETUP CURL" << endl;
    return -1;
  }*/


  Browse b;
  b.set_max_selections(1);
  b.set_extension(".log");
  b.set_selection_type(SelectionType::sel_all);
  auto selected_paths = b.start();

  if(selected_paths.size() <= 0){
    cout << "No file selected... exiting" << endl;
    return -1;
  }

  string folder = boost::filesystem::path(selected_paths[0]).parent_path().filename().string();

  for (auto file : selected_paths){

    vector<map<string, vector<double>>> data(chimera.devices.size());


    message msg;
    vector<string> lines;
    get_lines(file, &lines);
    Device* modified = nullptr;
    double prev_timestamp = -1;
    auto start_time = steady_clock::now();
    for(int i = 20; i < lines.size(); i++){
      if(!parse_message(lines [i], &msg))
        continue;

      modified = chimera.parse_message(msg.timestamp, msg.id, msg.data, msg.size);

      if(modified != nullptr){
        // Add message if elased enough time
        if(modified->timestamp - modified->helper_variable > MAX_DT){
          modified->helper_variable = modified->timestamp;

          string keys_str = modified->get_header(";");
          vector<string> keys = split(keys_str, ';');
          string values_str = modified->get_string(";");
          vector<string> values = split(values_str, ';');
          for(int j = 0; j < keys.size(); j++){
            try{
              data[modified->get_id()][keys[j]].push_back(stod(values[j]));
            }
            catch(int e){
              data[modified->get_id()][keys[j]].push_back(0.0);
            }
          }
        }
      }

      if(REAL_TIME == 1){
        if(prev_timestamp > 0){
          usleep((msg.timestamp - prev_timestamp)*1000000);
        }
        prev_timestamp = msg.timestamp;
        if(duration_cast<duration<double, milli>>(steady_clock::now() - start_time).count() > TIMEOUT){
          string j = pack_json(folder, data);
          send_json("http://127.0.0.1:8000/Dashboard/realTime/setData", j);
          setup_chimera_data(data);
          start_time = steady_clock::now();
        }
      }
    }

    if(REAL_TIME == false){
      string j = pack_json(folder, data);
      send_json("http://127.0.0.1:8000/Dashboard/setData", j);
      setup_chimera_data(data);
    }
  }

  return  0;
}


string pack_json(string name, vector<map<string, vector<double>>> data){
  std::stringstream ss;
  nlohmann::ordered_json all_data;
  for(auto device : chimera.devices){
    string keys_str = device->get_header(";");
    vector<string> keys = split(keys_str, ';');
    string values_str = device->get_string(";");
    vector<string> values = split(values_str, ';');
    all_data[device->get_name()] = {};
    for(int i = 0; i < keys.size(); i++){
      all_data[device->get_name()][keys[i]] = data[device->get_id()][keys[i]];
    }
  }
  all_data["name"] = name;
  all_data["data"] = all_data;
  all_data["object"] = all_data;

  ss << all_data;

  return ss.str();
}

int send_json(string url, string post){
  CURLcode res;
  struct curl_slist *slist1;

  slist1 = NULL;
  slist1 = curl_slist_append(slist1, "Content-Type: application/json");

  curl = curl_easy_init();

  if(!curl)
  return -1;

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POST, 1);

  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post.length());
  res = curl_easy_perform(curl);

  if(res != CURLE_OK)
    cout << "Error: " << curl_easy_strerror(res) << endl;

  curl_easy_cleanup(curl);
  return 0;
}

// "http://127.0.0.1:8000/Dashboard/realTime/setData"
// "https://driverless-configurator.herokuapp.com/Dashboard/setData"
int setup_curl(string url){

  //curl_easy_cleanup(curl);
}


void setup_chimera_data(vector<map<string, vector<double>>>& data){
  data.clear();
  data.resize(chimera.devices.size());
  for(auto device : chimera.devices){
    string keys_str = device->get_header(";");
    vector<string> keys = split(keys_str, ';');
    string values_str = device->get_string(";");
    vector<string> values = split(values_str, ';');
    for(int i = 0; i < keys.size(); i++){
      vector<double> empty_vec;
      data[device->get_id()][keys[i]] = empty_vec;
    }
  }
}
