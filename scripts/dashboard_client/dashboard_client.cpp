#include "dashboard_client.h"

void on_key_press(char& key)
{
  switch(key)
  {
    case 'a':
      renderer->MoveLeft();
    break;
    case 'd':
      renderer->MoveRight();
    break;
    case ' ':
      renderer->ToggleStartStop();
    break;
  }
}

int main(int argc, char* argv[]) {
    Client c;
    renderer = new Renderer(W, H);

    chimera_proto = new devices::Chimera();
    chimera_data = new ChimeraData(chimera_proto);

    c.set_on_message(&on_message);

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


    renderer->SetBackground(50, 40, 32);
    page1 = new Page1(W, H);
    page1->SetData(chimera_data);
    renderer->AddPage(page1);

    page2 = new Page2(W, H);
    page2->SetData(chimera_data);
    renderer->AddPage(page2);

    renderer->SetOnKeyPress(on_key_press);










    current_thread->join();
}


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
    renderer->GetPage()->SetData(chimera_data);
  }
}
