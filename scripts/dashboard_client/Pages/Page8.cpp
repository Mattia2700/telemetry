#include "Page8.h"

Page8::Page8(string name, int w, int h): Page(name, w, h)
{
  int margin = H/10;

  Box pos1{
    margin,
    margin,
    W/2 - 2*margin,
    H/2 - margin
  };
  Box pos2{
    W/2 + margin,
    margin,
    W/2 - 2*margin,
    H/2 - margin
  };
  margin = H/20;
  Box pos3{
    margin,
    H/2 + margin,
    W/3 - 2*margin,
    H/2 - 2*margin
  };
  Box pos4{
    W/3 + margin,
    H/2 + margin,
    W/3 - 2*margin,
    H/2 - 2*margin
  };
  Box pos5{
    W*2/3 + margin,
    H/2 + margin,
    W/3 - 2*margin,
    H/2 - 2*margin
  };

  value_text_box = new TextBox("Sensor Values", pos1);
  frequency_text_box = new TextBox("Sensor Frequency", pos2);
  ecu_state_text_box = new TextBox("ECU State", pos3);
  bms_state_text_box = new TextBox("BMS State", pos4);
  steer_state_text_box = new TextBox("Steering Wheel State", pos5);

  ecu_state_text_box->SetMaxLines(8);
  bms_state_text_box->SetMaxLines(8);
  steer_state_text_box->SetMaxLines(8);

  ui_elements.push_back(value_text_box);
  ui_elements.push_back(frequency_text_box);
  ui_elements.push_back(ecu_state_text_box);
  ui_elements.push_back(bms_state_text_box);
  ui_elements.push_back(steer_state_text_box);
};

int Page8::Draw()
{
  if(current_data == nullptr || !new_data)
    return 0;

  unique_lock<mutex> lck(mtx);

  background->setTo(background_color);

  ChimeraData* chim = (ChimeraData*)current_data;

  if(!SetTextData(chim))
    return 0;

  value_text_box->Draw(background);
  frequency_text_box->Draw(background);
  ecu_state_text_box->Draw(background);
  bms_state_text_box->Draw(background);
  steer_state_text_box->Draw(background);

  frame_count++;
  new_data = false;
  return 1;
}

int Page8::SetTextData(ChimeraData* chim)
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

  value_text_box->Clear();
  frequency_text_box->Clear();


  value_text_box->AppendLine({TITLE, "Inverter Left", title_color});
  frequency_text_box->AppendLine({TITLE, "Inverter Left", title_color});
  {
    size = chim->data->inverter_left_size();
    if(size > 1)
    {
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
      ret = 1;
    }
    else
    {
      if(ret != 1)
        ret = 0;
    }
  }

  value_text_box->AppendLine({TITLE, "Inverter Right", title_color});
  frequency_text_box->AppendLine({TITLE, "Inverter Right", title_color});
  {
    size = chim->data->inverter_right_size();
    if(size > 1){
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
      ret = 1;
    }
    else
    {
      if(ret != 1)
        ret = 0;
    }
  }

  {
    size = chim->data->ecu_state_size();
    if(size > 0)
    {
      for(int i = 0; i < size; i++)
      {
        auto ecu = chim->data->ecu_state(i);
        if(ecu.value() != last_ecu_state)
        {
          ecu_state_text_box->AppendLine({PARAGRAPH, ecu.value(), text_color});
          last_ecu_state = ecu.value();
        }
      }
      ret = 1;
    }
    else
    {
      if(ret != 1)
        ret = 0;
    }
  }
  {
    size = chim->data->bms_hv_state_size();
    if(size > 0)
    {
      for(int i = 0; i < size; i++)
      {
        auto bms = chim->data->bms_hv_state(i);
        if(last_bms_state != bms.value())
        {
          bms_state_text_box->AppendLine({PARAGRAPH, bms.value(), text_color});
          last_bms_state = bms.value();
        }
      }
      ret = 1;
    }
    else
    {
      if(ret != 1)
        ret = 0;
    }
  }
  {
    size = chim->data->steering_wheel_state_size();
    if(size > 0)
    {
      for(int i = 0; i < size; i++)
      {
        auto steer = chim->data->steering_wheel_state(i);
        if(last_steer_state != steer.value())
        {
          steer_state_text_box->AppendLine({PARAGRAPH, steer.value(), text_color});
          last_steer_state = steer.value();
        }
      }
      ret = 1;
    }
    else
    {
      if(ret != 1)
        ret = 0;
    }
  }

  return ret;
}
