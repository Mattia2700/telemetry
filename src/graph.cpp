#include "graph.h"

void Graph::PushData(vector<double> &x_, vector<vector<double>> &ys_)
{
  if(this->ys.size() != 0 &&this->ys.size() != ys_.size())
    return;

  this->x.insert(this->x.end(), x_.begin(), x_.end());
  CheckLength(&this->x);

  if(this->ys.size() < ys_.size())
    this->ys.resize(ys_.size());

  for(int i = 0; i < this->ys.size(); i++){
    ys[i].insert(this->ys[i].end(), ys_[i].begin(), ys_[i].end());
    CheckLength(&this->ys[i]);
  }
}

void Graph::Draw(Mat* img)
{
  if(x.size() == 0 || ys.size() == 0)
    return;

  DrawAxes(img);

  if(hovered){
    cv::circle(*img,
      Point(position.x+10, position.y+10),
      8,
      Scalar(0, 0, 255, 255),
      command_mode?-1:1
    );
  }

  float dx = 0.0;
  float dy = float(position.h) / (scaleY * 2)*-1;

  float cx = position.x;
  float cy = position.y + position.h/2;


  for(int i = 0; i < ys.size(); i++)
  {
    if(i < show_line.size() && show_line[i] == false)
      continue;

    if(colors.size() <= i)
      colors.push_back(GenerateColor());

    dx = float(position.w) / ys[i].size();

    for(int j = 1; j < ys[i].size(); j++)
    {
      cv::line(*img,
        cv::Point(((j-1) * dx) + cx, (ys[i][j-1] * dy) + cy),
        cv::Point(((j  ) * dx) + cx, (ys[i][j  ] * dy) + cy),
        colors[i],
        1,
        LINE_AA
      );
    }
  }
}

void Graph::DrawAxes(Mat* img)
{
  float scale = scaleY;
  if(scale > 15)
    scale = 10;

  float text_size = 0.35;
  auto font = FONT_HERSHEY_SIMPLEX;

  float dx = float(position.w) / x.size();
  float dy = float(position.h) / (scale * 2)*-1;

  float cx = position.x;
  float cy = position.y + position.h/2;

  stringstream s;
  s.precision(2);
  for(int i = -scale; i <= scale; i++)
  {
    s.str("");
    s << std::fixed << (scaleY/scale * i);
    Size size = cv::getTextSize(s.str(), font, text_size, 1, 0);
    cv::putText(*img,
      s.str(),
      cv::Point(position.x-size.width, cy + i*dy + size.height/2),
      font,
      text_size,
      cv::Scalar(255,255,255, 255),
      1,
      LINE_AA);

    cv::line(*img,
      cv::Point(position.x, cy + i*dy),
      cv::Point(position.x+position.w, cy + i*dy),
      i == 0 ? Scalar(255, 255, 255, 1) : Scalar(100, 100, 100, 1),
      1,
      LINE_8
    );
  }

  int x_sections = 10;
  int jump = (x.size()-1) / x_sections;
  dx = float(position.w) / x_sections;
  for(int i = 0; i < x_sections; i++)
  {
    s.str("");
    s << std::fixed << (x[i] - offset_x);
    Size size = cv::getTextSize(s.str(), font, text_size, 1, 0);
    cv::putText(*img,
      s.str(),
      cv::Point(cx + i*dx - size.width/2, position.y+position.h + size.height*1.5),
      font,
      text_size,
      cv::Scalar(255,255,255, 255),
      1,
      LINE_AA);

    cv::line(*img,
      cv::Point(position.x+dx*i, position.y),
      cv::Point(position.x+dx*i, position.y+position.h),
      i == 0 ? Scalar(255, 255, 255, 1) : Scalar(100, 100, 100, 1),
      1,
      LINE_8
    );
  }

  text_size = 0.7;
  Size title_size = cv::getTextSize(name, font, text_size, 2, 0);
  cv::putText(*img,
    name,
    cv::Point(position.x, position.y - 8),
    font,
    text_size,
    cv::Scalar(255,255,255, 255),
    1,
    LINE_AA);
  int x = title_size.width;
  text_size = 0.45;
  for(int i = 0; i < labels.size(); i++)
  {
    Size size = cv::getTextSize(labels[i], font, text_size, 1, 0);
    cv::putText(*img,
      labels[i],
      cv::Point(position.x+x, position.y - 8),
      font,
      text_size,
      colors[i],
      i==label_hover_idx?2:1,
      LINE_AA);
    x += size.width+5;
  }
}

void Graph::SetMaxLenght(int length)
{
  max_length = length;
}
void Graph::SetLabels(vector<string> vec)
{
  labels = vec;
  show_line.resize(labels.size(), true);

  while(colors.size() < labels.size())
    colors.push_back(GenerateColor());
}

void Graph::CheckLength(vector<double>* to_check)
{
  int oversize = to_check->size() - max_length;
  if(oversize <= 0)
    return;

  to_check->erase(to_check->begin(), to_check->begin()+oversize);
}

void Graph::SetScale(double x, double y)
{
  scaleX = x;
  scaleY = y;
}

void Graph::AutoResize(bool val)
{
  auto_resize = val;
}

void Graph::SetPosition(Box box)
{
  position = box;
}

Scalar Graph::GenerateColor()
{
  cv::Scalar color(
    (double)std::rand() / (RAND_MAX) * 215+40,
    (double)std::rand() / (RAND_MAX) * 215+40,
    (double)std::rand() / (RAND_MAX) * 215+40,
    255
  );
  return color;
}


void Graph::CommandMode(char c)
{
  switch(c)
  {
    case 'a':
      if(labels.size() == 0)
        return;
      label_hover_idx = (label_hover_idx-1)<0?labels.size()-1:(label_hover_idx-1);

    break;
    case 'd':
      if(labels.size() == 0)
        return;
      label_hover_idx = (label_hover_idx + 1) % labels.size();
    break;
    case 13:
      if(label_hover_idx == -1)
        label_hover_idx = 0;
      else
        show_line[label_hover_idx] = !show_line[label_hover_idx];
    break;
    case 27:
      label_hover_idx = -1;
      command_mode = false;
    break;
  }
}
