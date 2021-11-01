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

    string out;
    TextFormat::PrintToString(*chimera_proto, &out);
    cout << out << endl;
  }

}

int main(int argc, char* argv[]) {
    Client c;
    chimera_proto = new devices::Chimera();

    c.set_on_message(&on_message);

    std::string uri = "ws://localhost:8080";

    if (argc == 2) {
        uri = argv[1];
    }

    auto current_thread = c.run(uri);
    if(current_thread == nullptr)
      return -1;


    Document d;
    StringBuffer sb;
    Writer<StringBuffer> w(sb);
    rapidjson::Document::AllocatorType &alloc = d.GetAllocator();
    d.SetObject();
    d.AddMember("identifier", Value().SetString("client"), alloc);
    d.Accept(w);

    c.set_data(string(sb.GetString()));
    current_thread->join();
}
