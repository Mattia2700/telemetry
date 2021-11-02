#include "renderer.h"

int Page::instance_count = 0;

Renderer::Renderer(int w, int h)
{
  background = Mat(h, w, CV_8UC4, Scalar(0, 0, 0, 0));
  render_image = Mat(h, w, CV_8UC4, Scalar(0, 0, 0, 0));

  page_idx = 0;
  frame_rate = 30;

  render_thread = new thread(&Renderer::Render, this);
}
void Renderer::SetBackground(int R, int G, int B){
  background.setTo(cv::Scalar(R,G,B,0));
}
void Renderer::Kill()
{
  kill.store(true);
}

void Renderer::WaitForFinish()
{
  if(render_thread != nullptr)
    render_thread->join();
}

void Renderer::Pause()
{
  pause.store(1);
}
void Renderer::Start()
{
  pause.store(0);
}
void Renderer::ToggleStartStop()
{
  pause ^= 1;
}

void Renderer::AddPage(Page* page, int idx)
{
  if(idx == -1 || idx >= pages.size())
    pages.push_back(page);
  else
    pages.insert(pages.begin()+idx, page);
}
void Renderer::RemovePage(Page* page){
  for(int i = 0; i < pages.size(); i++){
    if(page == pages[i])
    {
      pages.erase(pages.begin()+i);
      break;
    }
  }
  if(page_idx > pages.size())
    page_idx = pages.size()-1;
}
void Renderer::RemovePage(int idx){
  if(idx == -1 || idx >= pages.size())
    pages.pop_back();
  else
    pages.erase(pages.begin()+idx);

  if(page_idx > pages.size())
    page_idx = pages.size()-1;
}

void Renderer::MoveLeft()
{
  page_idx --;
  if(page_idx < 0)
    page_idx = pages.size()-1;
}
void Renderer::MoveRight()
{
  page_idx = (page_idx + 1) % pages.size();
}
void Renderer::MoveToIndex(int idx)
{
  if(page_idx == -1)
    page_idx = pages.size()-1;
  else if(page_idx >= 0 && page_idx < pages.size())
    page_idx = idx;
}

void Renderer::Render()
{
  cv::namedWindow("Dashboard");

  // VideoWriter outputVideo;
  // outputVideo.open("NAME.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, Size(800, 800), true);
  //
  // if (!outputVideo.isOpened())
  // {
  //     cout  << "Could not open the output video for write: " << endl;
  //     return ;
  // }

  while(true)
  {
    if(pages.size() == 0)
    {
      usleep((1.0/frame_rate) * 1000000);
      continue;
    }
    current_page = pages[page_idx];

    current_page->Draw();

    Mat* page_img = current_page->GetImage();
    for(int i = 0; i < background.rows; i++)
    {
      for(int j = 0; j < background.cols; j++)
      {
        if(page_img->at<cv::Vec4b>(i, j)[3] == 0)
        {
          render_image.at<cv::Vec4b>(i,j) = background.at<cv::Vec4b>(i,j);
        }
        else
        {
          render_image.at<cv::Vec4b>(i,j) = page_img->at<cv::Vec4b>(i,j);
        }
      }
    }


    cv::imshow("Dashboard", render_image);
    // outputVideo << render_image;

    cv::waitKey((1.0/frame_rate)*1000);
  }
}

void Page::SetData(Data* data)
{
  mtx.lock();
  current_data = data;
  mtx.unlock();
}
Mat* Page::GetImage()
{
  return background;
}
void Page::SetBackground(int R, int G, int B){
  background_color = cv::Scalar(R,G,B, 0);
  background->setTo(background_color);
}

void Page1::Draw()
{
  if(current_data == nullptr)
    return;
  mtx.lock();

  background->setTo(background_color);

  ChimeraData* chim = (ChimeraData*)current_data;

  int accel_size = chim->data->accel_size();
  if(accel_size > 1)
  {
    int scale = chim->data->accel(0).scale();

    double xi = chim->data->accel(0).timestamp();
    double xf = chim->data->accel(accel_size-1).timestamp();

    float dx = float(this->W) / accel_size;
    float dy = float(this->H) / (scale * 2);

    float cx = 0;
    float cy = this->H/2;

    for(int i = -scale; i < scale; i++)
    {
      cv::line(*background,
        cv::Point(0, cy + i*dy),
        cv::Point(W, cy + i*dy),
        Scalar(100, 100, 100, 1),
        1,
        LINE_8
      );
    }

    for(int i = 1; i < accel_size; i++)
    {
      if( i >= chim->data->accel_size())
        continue;
      const devices::Imu& dev1 = chim->data->accel(i-1);
      const devices::Imu& dev2 = chim->data->accel(i);

      cv::line(*background,
        cv::Point(((i-1) * dx) + cx, (dev1.x() * dy) + cy),
        cv::Point(((i  ) * dx) + cx, (dev2.x() * dy) + cy),
        Scalar(0, 0, 255, 255),
        1,
        LINE_AA
      );
      cv::line(*background,
        cv::Point(((i-1) * dx) + cx, (dev1.y() * dy) + cy),
        cv::Point(((i  ) * dx) + cx, (dev2.y() * dy) + cy),
        Scalar(0, 255, 0, 255),
        1,
        LINE_AA
      );
      cv::line(*background,
        cv::Point(((i-1) * dx) + cx, (dev1.z() * dy) + cy),
        cv::Point(((i  ) * dx) + cx, (dev2.z() * dy) + cy),
        Scalar(255, 0, 0, 255),
        1,
        LINE_AA
      );
    }
  }

  mtx.unlock();
}
