#include "websocket.h"
#include <ostream>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
using namespace rapidjson;

int main(int argc, char* argv[]) {
    Client c;

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
    d.AddMember("identifier", Value().SetString("telemetry"), alloc);
    d.Accept(w);

    c.set_data(string(sb.GetString()));
    c.set_data("jmndfk");
    current_thread->join();
}
