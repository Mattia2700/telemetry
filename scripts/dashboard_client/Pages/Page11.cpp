#include "Page11.h"

Page11::Page11(string name, int w, int h): Page(name, w, h)
{
  /*
  int margin = H/10;

  Box pos1{
    margin,
    margin,
    (W/2) - 2*margin,
    H - 2*margin
  };
  Box pos2{
    margin + W/2,
    margin,
    (W/2) - 2*margin,
    H - 2*margin
  };

  gga_text_box = new TextBox("GPS GGA Data", pos1);
  vtg_text_box = new TextBox("GPS VTG Data", pos2);

  ui_elements.push_back(gga_text_box);
  ui_elements.push_back(vtg_text_box);*/
};

int Page11::Draw()
{
  /*
  if(current_data == nullptr || !new_data)
    return 0;

  unique_lock<mutex> lck(mtx);

  background->setTo(background_color);

  TelemetryConfData* tel = (TelemetryConfData*)current_data;

  SetTextData(tel);

  gga_text_box->Draw(background);

  new_data = false;*/
  return 1;
}

void Page11::SetTextData(TelemetryConfData* chim)
{
}
