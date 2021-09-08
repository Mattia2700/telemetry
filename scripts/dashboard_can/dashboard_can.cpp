#include "dashboard_can.h"

int main(){

  sockaddr_can addr;
  Can *can = new Can(CAN_DEVICE, &addr);
  int sock = can->open();
  if (sock < 0)
  {
    cout << get_colored("Failed creating/binding socket: " + string(CAN_DEVICE), 1) << endl;
    CAN_DEVICE = "can0";
    can = new Can(CAN_DEVICE, &addr);
    sock = can->open();
    if (sock < 0)
    {
      cout << get_colored("Failed creating/binding socket: " + string(CAN_DEVICE), 1) << endl;
      cout << "Exiting" << endl;
      return 0;
    }
  }
  cout << get_colored("Opened Socket: " + string(CAN_DEVICE), 6) << endl;

  can_frame message;
  vector<Device*> modifiedDevices;
  double prev_timestamp = -1;
  auto start_time = steady_clock::now();
  double timestamp = get_timestamp();
  while (true)
  {
    timestamp = get_timestamp();
    can->receive(&message);
    modifiedDevices = chimera.parse_message(timestamp, message.can_id, message.data, message.can_dlc);

    if(modifiedDevices.size() > 0){
      chimera.serialize_to_string(&serialized_string);
      // chimera.serialize_to_text(&serialized_string);
      // chimera.serialize_to_json(&serialized_string);
    }
    if(prev_timestamp > 0){
      usleep((timestamp - prev_timestamp)*1000000);
    }

    prev_timestamp = timestamp;

    if(duration_cast<duration<double, milli>>(steady_clock::now() - start_time).count() > TIMEOUT){
      start_time = steady_clock::now();
      // cout << serialized_string << endl;
      chimera.clear_serialized();
      send_text("http://127.0.0.1:8000/Dashboard/realTime/setData", serialized_string);
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

double get_timestamp()
{
  return duration_cast<duration<double, milli>>(system_clock::now().time_since_epoch()).count() / 1000;
}
