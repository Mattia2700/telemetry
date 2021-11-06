#include "Page8.h"

Page8::Page8(string name, int w, int h): Page(name, w, h)
{
  int margin = H/10;

  Box pos1{
    margin,
    margin,
    W/2 - margin,
    H - margin
  };
  Box pos2{
    W/2 + margin,
    margin,
    W/2 - margin,
    H - margin
  };

  value_text_box = new TextBox("Sensor Values", pos1);
  frequency_text_box = new TextBox("Sensor Frequency", pos2);

  ui_elements.push_back(value_text_box);
  ui_elements.push_back(frequency_text_box);
};

int Page8::Draw()
{
  if(current_data == nullptr || !new_data)
    return 0;

  mtx.lock();

  background->setTo(background_color);

  ChimeraData* chim = (ChimeraData*)current_data;

  if(!SetTextData(chim))
    return 0;

  value_text_box->Draw(background);
  frequency_text_box->Draw(background);

  mtx.unlock();
  frame_count++;
  new_data = false;
  return 1;
}

int Page8::SetTextData(ChimeraData* chim)
{
  // if(steer_size == 0)
  //   return 0;
  int size;
  double val;
  stringstream ss_val;
  stringstream ss_freq;
  ss_val.precision(3);
  ss_freq.precision(0);

  Scalar title_color(0, 255, 255, 255);
  Scalar text_color(255, 255, 255, 255);

  value_text_box->Clear();
  frequency_text_box->Clear();


  value_text_box->AppendLine({TITLE, "Inverter Left", title_color});
  frequency_text_box->AppendLine({TITLE, "Inverter Left", title_color});
  {
    size = chim->data->inverter_left_size();
    if(size < 2)
      return 0;
    auto inverter = chim->data->inverter_left(size - 1);

    ss_val.str(" ");
    ss_val << "temperature        -> " << fixed << inverter.temperature();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
    ss_val.str(" ");
    ss_val << "motor temperature  -> " << fixed << inverter.motor_temp();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
    ss_val.str(" ");
    ss_val << "torque             -> " << fixed << inverter.torque();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});

    auto inverter_ = chim->data->inverter_left(0);
    val = 1.0/((inverter.timestamp() - inverter_.timestamp())/size);
    ss_freq.str(" ");
    ss_freq << "Frequency [Hz] -> " << fixed << val;
    frequency_text_box->AppendLine({PARAGRAPH, ss_freq.str(), text_color});
    frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
    frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
  }

  value_text_box->AppendLine({TITLE, "Inverter Right", title_color});
  frequency_text_box->AppendLine({TITLE, "Inverter Right", title_color});
  {
    size = chim->data->inverter_right_size();
    if(size < 2)
      return 0;
    auto inverter = chim->data->inverter_right(size - 1);

    ss_val.str(" ");
    ss_val << "temperature        -> " << fixed << inverter.temperature();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
    ss_val.str(" ");
    ss_val << "motor temperature  -> " << fixed << inverter.motor_temp();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
    ss_val.str(" ");
    ss_val << "torque             -> " << fixed << inverter.torque();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});

    auto inverter_ = chim->data->inverter_right(0);
    val = 1.0/((inverter.timestamp() - inverter_.timestamp())/size);
    ss_freq.str(" ");
    ss_freq << "Frequency [Hz] -> " << fixed << val;
    frequency_text_box->AppendLine({PARAGRAPH, ss_freq.str(), text_color});
    frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
    frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
  }

  // value_text_box->AppendLine({TITLE, "BSM HV", title_color});
  // frequency_text_box->AppendLine({TITLE, "BSM HV", title_color});
  // {
  //   size = chim->data->bms_hv_size();
  //   cout << size << endl;
  //   if(size < 2)
  //     return 0;
  //   auto bms = chim->data->bms_hv(size - 1);
  //
  //   ss_val.str(" ");
  //   ss_val << "temperature -> " << fixed << bms.temperature();
  //   value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
  //   ss_val.str(" ");
  //   ss_val << "voltage     -> " << fixed << bms.voltage();
  //   value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
  //   ss_val.str(" ");
  //   ss_val << "current     -> " << fixed << bms.current();
  //   value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
  //
  //   auto bms_ = chim->data->bms_hv(0);
  //   val = 1.0/((bms.timestamp() - bms_.timestamp())/size);
  //   ss_freq.str(" ");
  //   ss_freq << "Frequency [Hz] -> " << fixed << val;
  //   frequency_text_box->AppendLine({PARAGRAPH, ss_freq.str(), text_color});
  //   frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
  //   frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
  // }

  // value_text_box->AppendLine({TITLE, "BMS LV", title_color});
  // frequency_text_box->AppendLine({TITLE, "BMS LV", title_color});
  // {
  //   size = chim->data->bms_lv_size();
  //   if(size < 2)
  //     return 0;
  //   auto bms = chim->data->bms_lv(size - 1);
  //
  //   ss_val.str(" ");
  //   ss_val << "temperature -> " << fixed << bms.temperature();
  //   value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
  //   ss_val.str(" ");
  //   ss_val << "voltage     -> " << fixed << bms.voltage();
  //   value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
  //
  //   auto bms_ = chim->data->bms_lv(0);
  //   val = 1.0/((bms.timestamp() - bms_.timestamp())/size);
  //   ss_freq.str(" ");
  //   ss_freq << "Frequency [Hz] -> " << fixed << val;
  //   frequency_text_box->AppendLine({PARAGRAPH, ss_freq.str(), text_color});
  //   frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
  // }
  return 1;
}
