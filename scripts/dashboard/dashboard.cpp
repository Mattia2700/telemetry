#include "dashboard.h"

#include <curl/curl.h>

int main(){

  Browse b;
  b.set_max_selections(1);
  b.set_extension(".log");
  b.set_selection_type(SelectionType::sel_all);
  auto selected_paths = b.start();

  if(selected_paths.size() <= 0){
    cout << "No file selected... exiting" << endl;
    return -1;
  }

  for (auto file : selected_paths){

    Chimera chimera;

    vector<map<string, vector<double>>> data(chimera.devices.size());

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


    message msg;
    vector<string> lines;
    get_lines(file, &lines);
    Device* modified = nullptr;
    for(int i = 20; i < lines.size() && i < 1000; i++){
      if(!parse_message(lines [i], &msg))
        continue;
      modified = chimera.parse_message(msg.timestamp, msg.id, msg.data, msg.size);


      if(modified != nullptr){
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
    all_data["object"] = all_data;

    ss << all_data;

    CURL *curl;
    CURLcode res;
    struct curl_slist *slist1;

    string post = ss.str();
    cout << post << endl;

    slist1 = NULL;
    slist1 = curl_slist_append(slist1, "Content-Type: application/json");

    curl = curl_easy_init();
    if(curl) {
      curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:8000/Dashboard/setData");
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());
      curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post.length());
      curl_easy_setopt(curl, CURLOPT_POST, 1);

      res = curl_easy_perform(curl);
      /* Check for errors */
      if(res != CURLE_OK)
        cout << "Error: " << curl_easy_strerror(res) << endl;

      /* always cleanup */
      curl_easy_cleanup(curl);
    }
  }

  return  0;
}
