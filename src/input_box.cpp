#include "input_box.h"



void InputBox::SetText(const input::Text& text_)
{
  text = text_;
}

void InputBox::Draw(Mat* img)
{
  float x = position.x;
  float y = position.y;
  float max_x = x + position.w;
  float max_y = y + position.h;

  Scalar color = Scalar(255, 255, 255, 255);
  if(command_mode)
  {
    color = Scalar(0, 0, 255, 255);
  }
  else if (hovered)
  {
    color = Scalar(255, 0, 0, 255);
  }
  cv::rectangle(*img,
    Point(x, y),
    Point(max_x, max_y),
    color,
    1
  );

  if(text.content == "")
    return;

  auto size = GetTextSize(text);
  y += position.h / 2;
  x += position.w / 2 - size.width / 2;

  cv::putText(*img,
    text.content,
    cv::Point(x, y),
    BASE_FONT_TYPE,
    input::FONT_SIZES[text.type],
    text.color,
    1,
    LINE_AA);
}

cv::Size InputBox::GetCharSize(input::TextType type)
{
  return cv::getTextSize("O",
    BASE_FONT_TYPE,
    input::FONT_SIZES[type],
    1,
    0
  );
}


cv::Size InputBox::GetTextSize(const input::Text& line)
{
  return cv::getTextSize(line.content,
    BASE_FONT_TYPE,
    input::FONT_SIZES[line.type],
    1,
    0
  );
}


void InputBox::Clear()
{
    text.content = "";
}


void InputBox::CommandMode(char c)
{
  if(c == 27)
  {
    command_mode = false;
  }
  else if(c == 13)
  {
    if(m_OnEnter)
      m_OnEnter(this, text.content);
  }
  else
  {
    if(m_OnInput)
      m_OnInput(this, c);
  }
}
