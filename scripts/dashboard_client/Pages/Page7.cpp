#include "Page7.h"

Page7::Page7(string name, int w, int h): Page(name, w, h)
{
  int margin = H/10;

  Box pos{
    margin,
    margin,
    W - margin,
    H - margin
  };

  text_box = new TextBox("Page7", pos);

  ui_elements.push_back(text_box);
};

void Page7::Draw()
{
  if(current_data == nullptr || !new_data)
    return;

  mtx.lock();

  background->setTo(background_color);

  ChimeraData* chim = (ChimeraData*)current_data;

  SetTextData(chim);

  text_box->Draw(background);

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
  stringstream ss;
  ss.precision(3);

  Scalar title_color(0, 255, 255, 255);
  Scalar text_color(255, 255, 255, 255);

  text_box->Clear();

  text_box->AppendLine({TITLE, "Pedals", title_color});
  {
    size = chim->data->pedal_size();
    auto pedal = chim->data->pedal(size - 1);
    ss.str(" ");
    ss << "throttle    -> " << fixed << pedal.throttle1();
    text_box->AppendLine({PARAGRAPH, ss.str(), text_color});
    ss.str(" ");
    ss << "brake front -> " << fixed << pedal.brake_front();
    text_box->AppendLine({PARAGRAPH, ss.str(), text_color});
    ss.str(" ");
    ss << "brake rear  -> " << fixed << pedal.brake_rear();
    text_box->AppendLine({PARAGRAPH, ss.str(), text_color});
  }

  text_box->AppendLine({TITLE, "Encoder Left", title_color});
  {
    size = chim->data->encoder_left_size();
    auto encoder = chim->data->encoder_left(size - 1);
    ss.str(" ");
    ss << "rads       -> " << fixed << encoder.rads();
    text_box->AppendLine({PARAGRAPH, ss.str(), text_color});
    ss.str(" ");
    ss << "km         -> " << fixed << encoder.km();
    text_box->AppendLine({PARAGRAPH, ss.str(), text_color});
    ss.str(" ");
    ss << "rotations  -> " << fixed << encoder.rotations();
    text_box->AppendLine({PARAGRAPH, ss.str(), text_color});
  }

  text_box->AppendLine({TITLE, "Encoder Right", title_color});
  {
    size = chim->data->encoder_right_size();
    auto encoder = chim->data->encoder_right(size - 1);
    ss.str(" ");
    ss << "rads       -> " << fixed << encoder.rads();
    text_box->AppendLine({PARAGRAPH, ss.str(), text_color});
    ss.str(" ");
    ss << "km         -> " << fixed << encoder.km();
    text_box->AppendLine({PARAGRAPH, ss.str(), text_color});
    ss.str(" ");
    ss << "rotations  -> " << fixed << encoder.rotations();
    text_box->AppendLine({PARAGRAPH, ss.str(), text_color});
  }

  text_box->AppendLine({TITLE, "Steer", title_color});
  {
    size = chim->data->steer_size();
    auto steer = chim->data->steer(size - 1);
    ss.str(" ");
    ss << "angle -> " << fixed << steer.angle();
    text_box->AppendLine({PARAGRAPH, ss.str(), text_color});
  }

  text_box->AppendLine({TITLE, "Accel", title_color});
  {
    size = chim->data->accel_size();
    auto accel = chim->data->accel(size - 1);
    ss.str(" ");
    ss << "x -> " << fixed << accel.x();
    text_box->AppendLine({PARAGRAPH, ss.str(), text_color});
    ss.str(" ");
    ss << "y -> " << fixed << accel.y();
    text_box->AppendLine({PARAGRAPH, ss.str(), text_color});
    ss.str(" ");
    ss << "z -> " << fixed << accel.z();
    text_box->AppendLine({PARAGRAPH, ss.str(), text_color});
  }
  text_box->AppendLine({TITLE, "Gyro", title_color});
  {
    size = chim->data->gyro_size();
    auto gyro = chim->data->gyro(size - 1);
    ss.str(" ");
    ss << "x -> " << fixed << gyro.x();
    text_box->AppendLine({PARAGRAPH, ss.str(), text_color});
    ss.str(" ");
    ss << "y -> " << fixed << gyro.y();
    text_box->AppendLine({PARAGRAPH, ss.str(), text_color});
    ss.str(" ");
    ss << "z -> " << fixed << gyro.z();
    text_box->AppendLine({PARAGRAPH, ss.str(), text_color});
  }
}
