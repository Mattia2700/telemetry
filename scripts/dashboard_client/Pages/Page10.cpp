#include "Page10.h"

Page10::Page10(string name, int w, int h): Page(name, w, h)
{
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
  ui_elements.push_back(vtg_text_box);
};

int Page10::Draw()
{
  if(current_data == nullptr || !new_data)
    return 0;

  unique_lock<mutex> lck(mtx);

  background->setTo(background_color);

  ChimeraData* chim = (ChimeraData*)current_data;

  if(!SetTextData(chim))
    return 0;

  gga_text_box->Draw(background);
  vtg_text_box->Draw(background);

  new_data = false;
  return 1;
}

int Page10::SetTextData(ChimeraData* chim)
{
  int ret = 0;

  int size;
  double val;
  string state;
  stringstream ss_val;
  stringstream ss_freq;
  ss_val.precision(3);
  ss_freq.precision(0);

  Scalar title_color(0, 255, 255, 255);
  Scalar text_color(255, 255, 255, 255);

  bool found_gga = false;
  bool found_vtg = false;
  for(int i = chim->data->gps_size()-1; i >= 0; i--)
  {
    auto gps = chim->data->gps(i);
    if(!found_gga)
    {
      if(gps.msg_type() == "GGA")
      {
        found_gga = true;

        gga_text_box->Clear();
        gga_text_box->AppendLine({TITLE, "GGA Data", title_color});
        ss_val.str(" ");
        ss_val << "message type        -> " << fixed << gps.msg_type();
        gga_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
        ss_val.str(" ");
        ss_val << "time        -> " << fixed << gps.time();
        gga_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
        ss_val.str(" ");
        ss_val << "latitude        -> " << fixed << gps.latitude();
        gga_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
        ss_val.str(" ");
        ss_val << "longitude        -> " << fixed << gps.longitude();
        gga_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
        ss_val.str(" ");
        ss_val << "altitude        -> " << fixed << gps.altitude();
        gga_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
        ss_val.str(" ");
        ss_val << "fix        -> " << fixed << gps.fix();
        gga_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
        ss_val.str(" ");
        ss_val << "satellites        -> " << fixed << gps.satellites();
        gga_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
        ss_val.str(" ");
        ss_val << "fix state        -> " << fixed << gps.fix_state();
        gga_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
        ss_val.str(" ");
        ss_val << "age of correction        -> " << fixed << gps.age_of_correction();
        gga_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
      }
    }
    if(!found_vtg)
    {
      if(gps.msg_type() == "VTG")
      {
        cout << gps.msg_type() << endl;
        found_vtg = true;
        vtg_text_box->Clear();
        vtg_text_box->AppendLine({TITLE, "VTG Data", title_color});
        ss_val.str(" ");
        ss_val << "message type           -> " << fixed << gps.msg_type();
        vtg_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
        ss_val.str(" ");
        ss_val << "couse over ground deg  -> " << fixed << gps.course_over_ground_degrees();
        vtg_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
        ss_val.str(" ");
        ss_val << "couse over ground mag  -> " << fixed << gps.course_over_ground_degrees_magnetic();
        vtg_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
        ss_val.str(" ");
        ss_val << "Km/h                   -> " << fixed << gps.speed_kmh();
        vtg_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
        ss_val.str(" ");
        ss_val << "mode                   -> " << fixed << gps.mode();
        vtg_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
      }
    }
  }
  ret = 1;

  return ret;
}
