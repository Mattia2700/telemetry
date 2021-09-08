#include "dashboard.h"

Document j;

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
    message msg;
    vector<string> lines;
    get_lines(file, &lines);
    vector<Device*> modifiedDevices;
    double prev_timestamp = -1;
    auto start_time = steady_clock::now();
    for(int i = 20; i < lines.size(); i++){
      if(!parse_message(lines [i], &msg))
        continue;

      modifiedDevices = chimera.parse_message(msg.timestamp, msg.id, msg.data, msg.size);

      if(modifiedDevices.size() > 0){
        chimera.serialize_to_string(&serialized_string);
        // chimera.serialize_to_text(&serialized_string);
        // chimera.serialize_to_json(&serialized_string);
      }

      if(REAL_TIME == true){
        if(prev_timestamp > 0){
          usleep((msg.timestamp - prev_timestamp)*1000000);
        }

        prev_timestamp = msg.timestamp;

        if(duration_cast<duration<double, milli>>(steady_clock::now() - start_time).count() > TIMEOUT){
          start_time = steady_clock::now();
          // cout << serialized_string << endl;
          chimera.clear_serialized();
          send_text("http://127.0.0.1:8000/Dashboard/realTime/setData", serialized_string);
        }
      }
    }

    if(REAL_TIME == false){
      send_text("http://127.0.0.1:8000/Dashboard/realTime/setData", serialized_string);
      chimera.clear_serialized();
    }
  }

  return  0;
}

int send_text(string url, string data){
  CURLcode res;
  struct curl_slist *slist1;

  slist1 = NULL;
  slist1 = curl_slist_append(slist1, "Content-Type: text/plain");

  curl = curl_easy_init();

  if(!curl)
  return -1;

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POST, 1);

  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
  res = curl_easy_perform(curl);

  // if(res != CURLE_OK)
  //   cout << "Error: " << curl_easy_strerror(res) << endl;

  curl_easy_cleanup(curl);
  return 0;
}

// "http://127.0.0.1:8000/Dashboard/realTime/setData"
// "https://driverless-configurator.herokuapp.com/Dashboard/setData"
int setup_curl(string url){

  //curl_easy_cleanup(curl);
}
