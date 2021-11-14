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
    Page1 page1("Throttle-Speed", W, H);
    page1.SetData(chimera_data);
    renderer->AddPage(&page1);

    Page2 page2("Accel-Gyro", W, H);
    page2.SetData(chimera_data);
    renderer->AddPage(&page2);

    Page3 page3("BMS", W, H);
    page3.SetData(chimera_data);
    renderer->AddPage(&page3);

    Page4 page4("Inverter", W, H);
    page4.SetData(chimera_data);
    renderer->AddPage(&page4);

    Page5 page5("Throttle-Brake", W, H);
    page5.SetData(chimera_data);
    renderer->AddPage(&page5);

    Page6 page6("Steer-Gyro", W, H);
    page6.SetData(chimera_data);
    renderer->AddPage(&page6);

    Page7 page7("Sensors", W, H);
    page7.SetData(chimera_data);
    renderer->AddPage(&page7);

    Page8 page8("Inverters", W, H);
    page8.SetData(chimera_data);
    renderer->AddPage(&page8);

    renderer->SetOnKeyPress(on_key_press);

    renderer->Start();









    current_thread->join();
}


void on_message(client* cli, websocketpp::connection_hdl hdl, message_ptr msg){
  Document d;
  StringBuffer sb;
  Writer<StringBuffer> w(sb);
  rapidjson::Document::AllocatorType &alloc = d.GetAllocator();

  chimera_proto->Clear();

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
    // chimera_data->PushData(chimera_proto);
    renderer->GetPage()->SetData(chimera_data);

    int size = chimera_proto->encoder_right_size();
    if(size > 0)
    {
      double timestamp_received = chimera_proto->encoder_right(size-1).timestamp();
      double timestamp = duration_cast<duration<double, milli>>(system_clock::now().time_since_epoch()).count() / 1000;

      cout << "\rPacket Size: " << HumanReadable(msg->get_payload().size()) << " Estimated latency: " << std::fixed << setprecision(3) << (timestamp - timestamp_received) << flush;
    }
  }
}


string HumanReadable(uintmax_t size){
  auto mantissa = size;
  int i{};
  for (; mantissa >= 1024.; mantissa /= 1024., ++i) { }
  mantissa = std::ceil(mantissa * 10.) / 10.;
  return to_string(mantissa) + "BKMGTPE"[i];
}
