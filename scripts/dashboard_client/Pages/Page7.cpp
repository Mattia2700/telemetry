#include "Page7.h"

Page7::Page7(string name, int w, int h): Page(name, w, h)
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

void Page7::Draw()
{
  if(current_data == nullptr || !new_data)
    return;

  mtx.lock();

  background->setTo(background_color);

  ChimeraData* chim = (ChimeraData*)current_data;

  SetTextData(chim);

  value_text_box->Draw(background);
  frequency_text_box->Draw(background);

  mtx.unlock();
  frame_count++;
  new_data = false;
}

void Page7::SetTextData(ChimeraData* chim)
{
  // if(steer_size == 0)
  //   return;
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

  value_text_box->AppendLine({TITLE, "Pedals", title_color});
  frequency_text_box->AppendLine({TITLE, "Pedals", title_color});
  {
    size = chim->data->pedal_size();
    if(size < 2)
      return;
    auto pedal = chim->data->pedal(size - 1);

    ss_val.str(" ");
    ss_val << "throttle    -> " << fixed << pedal.throttle1();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
    ss_val.str(" ");
    ss_val << "brake front -> " << fixed << pedal.brake_front();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
    ss_val.str(" ");
    ss_val << "brake rear  -> " << fixed << pedal.brake_rear();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});

    auto pedal_ = chim->data->pedal(0);
    val = 1.0/((pedal.timestamp() - pedal_.timestamp())/size);
    ss_freq.str(" ");
    ss_freq << "Frequency [Hz] -> " << fixed << val;
    frequency_text_box->AppendLine({PARAGRAPH, ss_freq.str(), text_color});
    frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
    frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
  }

  value_text_box->AppendLine({TITLE, "Encoder Left", title_color});
  frequency_text_box->AppendLine({TITLE, "Encoder Left", title_color});
  {
    size = chim->data->encoder_left_size();
    if(size < 2)
      return;
    auto encoder = chim->data->encoder_left(size - 1);

    ss_val.str(" ");
    ss_val << "rads       -> " << fixed << encoder.rads();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
    ss_val.str(" ");
    ss_val << "km         -> " << fixed << encoder.km();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
    ss_val.str(" ");
    ss_val << "rotations  -> " << fixed << encoder.rotations();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});

    auto encoder_ = chim->data->encoder_left(0);
    val = 1.0/((encoder.timestamp() - encoder_.timestamp())/size);
    ss_freq.str(" ");
    ss_freq << "Frequency [Hz] -> " << fixed << val;
    frequency_text_box->AppendLine({PARAGRAPH, ss_freq.str(), text_color});
    frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
    frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
  }

  value_text_box->AppendLine({TITLE, "Encoder Right", title_color});
  frequency_text_box->AppendLine({TITLE, "Encoder Right", title_color});
  {
    size = chim->data->encoder_right_size();
    if(size < 2)
      return;
    auto encoder = chim->data->encoder_right(size - 1);

    ss_val.str(" ");
    ss_val << "rads       -> " << fixed << encoder.rads();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
    ss_val.str(" ");
    ss_val << "km         -> " << fixed << encoder.km();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
    ss_val.str(" ");
    ss_val << "rotations  -> " << fixed << encoder.rotations();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});

    auto encoder_ = chim->data->encoder_right(0);
    val = 1.0/((encoder.timestamp() - encoder_.timestamp())/size);
    ss_freq.str(" ");
    ss_freq << "Frequency [Hz] -> " << fixed << val;
    frequency_text_box->AppendLine({PARAGRAPH, ss_freq.str(), text_color});
    frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
    frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
  }

  value_text_box->AppendLine({TITLE, "Steer", title_color});
  frequency_text_box->AppendLine({TITLE, "Steer", title_color});
  {
    size = chim->data->steer_size();
    if(size < 2)
      return;
    auto steer = chim->data->steer(size - 1);
    ss_val.str(" ");
    ss_val << "angle -> " << fixed << steer.angle();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});

    auto steer_ = chim->data->steer(0);
    val = 1.0/((steer.timestamp() - steer_.timestamp())/size);
    ss_freq.str(" ");
    ss_freq << "Frequency [Hz] -> " << fixed << val;
    frequency_text_box->AppendLine({PARAGRAPH, ss_freq.str(), text_color});
  }

  value_text_box->AppendLine({TITLE, "Accel", title_color});
  frequency_text_box->AppendLine({TITLE, "Accel", title_color});
  {
    size = chim->data->accel_size();
    if(size < 2)
      return;
    auto accel = chim->data->accel(size - 1);

    ss_val.str(" ");
    ss_val << "x -> " << fixed << accel.x();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
    ss_val.str(" ");
    ss_val << "y -> " << fixed << accel.y();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
    ss_val.str(" ");
    ss_val << "z -> " << fixed << accel.z();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});

    auto accel_ = chim->data->accel(0);
    val = 1.0/((accel.timestamp() - accel_.timestamp())/size);
    ss_freq.str(" ");
    ss_freq << "Frequency [Hz] -> " << fixed << val;
    frequency_text_box->AppendLine({PARAGRAPH, ss_freq.str(), text_color});
    frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
    frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
  }

  value_text_box->AppendLine({TITLE, "Gyro", title_color});
  frequency_text_box->AppendLine({TITLE, "Gyro", title_color});
  {
    size = chim->data->gyro_size();
    if(size < 2)
      return;
    auto gyro = chim->data->gyro(size - 1);

    ss_val.str(" ");
    ss_val << "x -> " << fixed << gyro.x();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
    ss_val.str(" ");
    ss_val << "y -> " << fixed << gyro.y();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});
    ss_val.str(" ");
    ss_val << "z -> " << fixed << gyro.z();
    value_text_box->AppendLine({PARAGRAPH, ss_val.str(), text_color});

    auto gyro_ = chim->data->gyro(0);
    val = 1.0/((gyro.timestamp() - gyro_.timestamp())/size);
    ss_freq.str(" ");
    ss_freq << "Frequency [Hz] -> " << fixed << val;
    frequency_text_box->AppendLine({PARAGRAPH, ss_freq.str(), text_color});
    frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
    frequency_text_box->AppendLine({PARAGRAPH, " ", text_color});
  }
}
