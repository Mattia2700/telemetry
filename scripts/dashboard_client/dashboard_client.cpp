#include "dashboard_client.h"

#include "renderer.h"


#include "websocket.h"
#include <ostream>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
using namespace rapidjson;

#include "devices.pb.h"
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/json_util.h>
using namespace google::protobuf;
using namespace google::protobuf::util;

typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

devices::Chimera* chimera_proto;
ChimeraData* chimera_data;
Page1* page1;

int W = 800, H = 800;

void on_message(client* cli, websocketpp::connection_hdl hdl, message_ptr msg){
  Document d;
  StringBuffer sb;
  Writer<StringBuffer> w(sb);
  rapidjson::Document::AllocatorType &alloc = d.GetAllocator();

  d.Parse(msg->get_payload().c_str(), msg->get_payload().size());
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

    chimera_data->SetData(chimera_proto);
    page1->SetData(chimera_data);

    // string out;
    // TextFormat::PrintToString(*chimera_proto, &out);
    // cout << out << endl;
  }
}

int main(int argc, char* argv[]) {
    Client c;
    Renderer renderer(W, H);

    chimera_proto = new devices::Chimera();
    chimera_data = new ChimeraData(chimera_proto);

    c.set_on_message(&on_message);

    std::string uri = "ws://localhost:8080";

    auto current_thread = c.run(uri);
    if(current_thread == nullptr)
      return -1;


    // Send to server """authentication""" so sign in as client
    Document d;
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);
    rapidjson::Document::AllocatorType &alloc = d.GetAllocator();
    d.SetObject();
    d.AddMember("identifier", Value().SetString("client"), alloc);
    d.Accept(writer);
    c.set_data(string(sb.GetString()));



    renderer.SetBackground(50, 40, 32);
    page1 = new Page1(W, H);
    page1->SetData(chimera_data);
    renderer.AddPage(page1);










    current_thread->join();
}
