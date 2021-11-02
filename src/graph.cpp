#include "graph.h"

void Graph::PushData(vector<double> x_, vector<vector<double>> ys_)
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

  float dx = float(position.w) / x.size();
  float dy = float(position.h) / (scaleY * 2);

  float cx = position.x;
  float cy = position.y + position.h/2;


  for(int i = 1; i < x.size(); i++)
  {
    for(int j = 0; j < ys.size(); j++)
    {
      if(line_colors.size() <= j)
        line_colors.push_back(GenerateColor());

      cv::line(*img,
        cv::Point(((i-1) * dx) + cx, (ys[j][i-1] * dy) + cy),
        cv::Point(((i  ) * dx) + cx, (ys[j][i  ] * dy) + cy),
        line_colors[j],
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

  float dx = float(position.w) / x.size();
  float dy = float(position.h) / (scale * 2);

  float cx = position.x;
  float cy = position.y + position.h/2;

  stringstream s;
  s.precision(2);
  for(int i = -scale; i <= scale; i++)
  {
    s.str("");
    s << std::fixed << (scaleY/scale * i);
    Size size = cv::getTextSize(s.str(), FONT_HERSHEY_SIMPLEX, 0.4, 1, 0);
    cv::putText(*img,
      s.str(),
      cv::Point(position.x-size.width, cy + i*dy + size.height/2),
      cv::FONT_HERSHEY_SIMPLEX,
      0.4,
      cv::Scalar(255,255,255, 255),
      1,
      0);

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
    Size size = cv::getTextSize(s.str(), FONT_HERSHEY_SIMPLEX, 0.4, 1, 0);
    cv::putText(*img,
      s.str(),
      cv::Point(cx + i*dx - size.width/2, position.y+position.h + size.height*1.5),
      cv::FONT_HERSHEY_SIMPLEX,
      0.4,
      cv::Scalar(255,255,255, 255),
      1,
      0);

    cv::line(*img,
      cv::Point(position.x+dx*i, position.y),
      cv::Point(position.x+dx*i, position.y+position.h),
      i == 0 ? Scalar(255, 255, 255, 1) : Scalar(100, 100, 100, 1),
      1,
      LINE_8
    );
  }
}

void Graph::SetMaxLenght(int length)
{
  max_length = length;
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
    (double)std::rand() / (RAND_MAX) * 255,
    (double)std::rand() / (RAND_MAX-1) * 255,
    (double)std::rand() / (RAND_MAX-2) * 255,
    255
  );
  return color;
}
