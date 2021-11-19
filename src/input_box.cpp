#include "input_box.h"



void InputBox::SetText(const Text& text_)
{
  text = text_;
}

void InputBox::Draw(Mat* img)
{
  float x = position.x;
  float y = position.y;
  float max_x = x + position.w;
  float max_y = y + position.h;

  cv::rectangle(*img,
    Point(x, y),
    Point(max_x, max_y),
    Scalar(255, 255, 255, 255),
    1
  );

  y += GetCharSize(text.type).height*0.7;
  if(text.type == TITLE)
    y += GetCharSize(text.type).height*0.9;

  cv::putText(*img,
    text.content,
    cv::Point(x, y),
    BASE_FONT_TYPE,
    FONT_SIZES[text.type],
    text.color,
    1,
    LINE_AA);

}


cv::Size InputBox::GetCharSize(TextType type)
{
  return cv::getTextSize("O",
    BASE_FONT_TYPE,
    FONT_SIZES[type],
    1,
    0
  );
}


cv::Size InputBox::GetTextSize(const Text& line)
{
  return cv::getTextSize(line.content,
    BASE_FONT_TYPE,
    FONT_SIZES[line.type],
    1,
    0
  );
}


void InputBox::Clear()
{
    text.content = "";
}
