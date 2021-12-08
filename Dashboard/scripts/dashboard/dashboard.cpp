#include "dashboard.h"

#include "Application.h"

#include "Page1.h"

#include "proto_deserializer.h"

float W = 1200;
float H = 800;

int main(int argc, char* argv[]) {

  ws_client = new Client();
  chimera_proto = new devices::Chimera();

  chimera_deserializer = new ChimeraDeserializer();
  chimera_deserializer->SetMaxDeviceSize(1000);
  chimera_data = new ChimeraData(chimera_deserializer);

  ws_client->set_on_message(&on_message);
  auto current_thread = ws_client->run(uri);
  if(current_thread == nullptr)
    return -1;



  Application app(W, H, "Dashboard");
  if(app.Init() < 0)
    return -1;

  Page1 page1("Page1", W, H);
  page1.SetData(chimera_data);
  app.PushPage(&page1);

  
  
  app.Run();
  
}



void on_message(client* cli, websocketpp::connection_hdl hdl, message_ptr msg){
  Document d;
  StringBuffer sb;
  Writer<StringBuffer> w(sb);
  rapidjson::Document::AllocatorType &alloc = d.GetAllocator();

  chimera_proto->Clear();

  ParseResult ok = d.Parse(msg->get_payload().c_str(), msg->get_payload().size());
  if(!ok)
  {
    return;
  }
  if(d["type"] == "update_data"){
    Value::MemberIterator itr = d.FindMember("data");
    string n = itr->name.GetString();
    Value& v = itr->value;
    SizeType len = v.GetStringLength();
    auto ch = v.GetString();
    string data = string(ch, len);

    if(chimera_proto->ParseFromString(data) == 0)
    {
      cout << "Failed deserialization" << endl;
      return;
    }

    chimera_deserializer->Deserialize(chimera_proto);
    chimera_data->data = chimera_deserializer;
    chimera_data->newData = true;

    double timestamp_received = d["timestamp"].GetDouble();
    double timestamp = duration_cast<duration<double, milli>>(system_clock::now().time_since_epoch()).count() / 1000;


    cout << "\rPacket Size: " << HumanReadable(msg->get_payload().size()) << " Estimated latency: " <<
    std::fixed << setprecision(3) << (timestamp - timestamp_received) << flush;
  }
}


string HumanReadable(uintmax_t size){
  auto mantissa = size;
  int i{};
  for (; mantissa >= 1024.; mantissa /= 1024., ++i) { }
  mantissa = std::ceil(mantissa * 10.) / 10.;
  return to_string(mantissa) + "BKMGTPE"[i];
}