#include "text_box.h"


void TextBox::SetLines(const vector<Text>& text_)
{
  text.insert(text.end(), text_.begin(), text_.end());
  CheckLinesSize();
}

void TextBox::AppendLine(const Text& line)
{
  text.push_back(line);
  CheckLinesSize();
}

void TextBox::Draw(Mat* img)
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

  y += GetCharSize(TITLE).height*1.5;
  int name_x = x + position.w/2 - GetTextSize(Text{TITLE, name}).width / 2;
  cv::putText(*img,
    name,
    cv::Point(name_x, y),
    BASE_FONT_TYPE,
    FONT_SIZES[TITLE],
    Scalar(250, 250, 80 , 255),
    1,
    LINE_AA);

  x += GetCharSize(TITLE).width*1.5;
  y += GetCharSize(TITLE).height*1.5;

  for(int i = 0; i < text.size(); i++)
  {
    Text& line = text[i];
    y += GetCharSize(line.type).height*0.7;
    if(line.type == TITLE)
      y += GetCharSize(line.type).height*0.9;

    cv::putText(*img,
      line.content,
      cv::Point(x, y),
      BASE_FONT_TYPE,
      FONT_SIZES[line.type],
      line.color,
      1,
      LINE_AA);

    y += GetTextSize(line).height;
  }
}

void TextBox::WrapLine(Text& line)
{
  cv::Size char_size = GetCharSize(line.type);
  cv::Size size = GetTextSize(line);
  int chunks = 1;
  if(size.width > position.w)
  {
    chunks = ceil(float(size.width) / position.w);
    int chars = position.w / char_size.width;
    size_t pos = 0;

    for(int i = 0; i < chunks; i++)
    {
      // Getting only current line
      string current_line = line.content.substr(pos, chars);

      // Find last end of word
      size_t pos1 = current_line.find_last_of(" " );
      size_t pos2 = current_line.find_last_of("\n");
      size_t pos3 = current_line.find_last_of("\t");

      pos = pos1 > pos2 ? pos1 : pos2;
      pos = pos  > pos3 ? pos  : pos3;

      if(pos != string::npos)
      {
        // Chcking if the last word is not too long
      }
    }
  }
}

cv::Size TextBox::GetCharSize(TextType type)
{
  return cv::getTextSize("O",
    BASE_FONT_TYPE,
    FONT_SIZES[type],
    1,
    0
  );
}


cv::Size TextBox::GetTextSize(const Text& line)
{
  return cv::getTextSize(line.content,
    BASE_FONT_TYPE,
    FONT_SIZES[line.type],
    1,
    0
  );
}


void TextBox::Clear()
{
  text.clear();
}

void TextBox::SetMaxLines(int max_lines_)
{
  max_lines = max_lines_;
}

void TextBox::CheckLinesSize()
{
  if(text.size() > max_lines && max_lines > 0)
    text.erase(text.begin(), text.begin() + (text.size() - max_lines));
}
