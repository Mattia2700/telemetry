#include "Page11.h"

Page11::Page11(string name, int w, int h): Page(name, w, h)
{
  int margin = H/10;
  int width = W / 10;
  int height = 80;
  int padding = 8;

  Box pos1{
    margin,
    margin,
    width,
    height
  };
  Box pos2{
    margin,
    margin + height + padding,
    width,
    height
  };
  Box pos3{
    margin,
    margin + 2 * (height + padding),
    width,
    height
  };

  pilot_ib = new InputBox("Pilot", pos1);
  circuit_ib = new InputBox("Circuit", pos2);
  race_ib = new InputBox("Race", pos3);

  pilot_ib->SetOnInput(&Page11::on_input, this);
  circuit_ib->SetOnInput(&Page11::on_input, this);
  race_ib->SetOnInput(&Page11::on_input, this);

  pilot_ib->SetOnEnter(&Page11::on_enter, this);
  circuit_ib->SetOnEnter(&Page11::on_enter, this);
  race_ib->SetOnEnter(&Page11::on_enter, this);

  ui_elements.push_back(pilot_ib);
  ui_elements.push_back(circuit_ib);
  ui_elements.push_back(race_ib);

  config.pilot = 0;
  config.circuit = 0;
  config.race = 0;
};

int Page11::Draw()
{

  unique_lock<mutex> lck(mtx);

  background->setTo(background_color);

  SetTextData();

  pilot_ib->Draw(background);
  circuit_ib->Draw(background);
  race_ib->Draw(background);

  new_data = false;
  return 1;
}

void Page11::SetTextData()
{
  input::Text t;
  t.type = input::PARAGRAPH;
  t.color = cv::Scalar(255,255,255,255);

  t.content = PILOTS[config.pilot];
  pilot_ib->SetText(t);
  t.content = CIRCUITS[config.circuit];
  circuit_ib->SetText(t);
  t.content = RACES[config.race];
  race_ib->SetText(t);
}

void Page11::on_enter(UIElement* element, string value)
{
  Document d;
  StringBuffer sb;
  Writer<StringBuffer> writer(sb);
  rapidjson::Document::AllocatorType &alloc = d.GetAllocator();
  d.SetObject();
  d.AddMember("type", Value().SetString("set_telemetry_config"), alloc);
  Value val;
  val.SetObject();
  {
    val.AddMember("pilot", config.pilot, alloc);
    val.AddMember("circuit", config.circuit, alloc);
    val.AddMember("race", config.race, alloc);
  }

  d.AddMember("data", val, alloc);
  d.Accept(writer);

  if(m_OnEnter)
  {
    m_OnEnter(string(sb.GetString()));
  }
}

void Page11::on_input(UIElement* element, char c)
{
  if(c == 'd')
  {
    if(element == pilot_ib)
      config.pilot ++;
    else if(element == circuit_ib)
      config.circuit ++;
    else if(element == race_ib)
      config.race ++;
  }
  else if(c == 'a')
  {
    if(element == pilot_ib)
      config.pilot --;
    else if(element == circuit_ib)
      config.circuit --;
    else if(element == race_ib)
      config.race --;
  }
  if (config.pilot < 0)
    config.pilot = PILOTS.size() - 1;
  else if(config.pilot >= PILOTS.size())
    config.pilot = 0;

  if (config.circuit < 0)
    config.circuit = CIRCUITS.size() - 1;
  else if(config.circuit >= CIRCUITS.size())
    config.circuit = 0;

  if (config.race < 0)
    config.race = RACES.size() - 1;
  else if(config.race >= RACES.size())
    config.race = 0;
}
