#include "renderer.h"

int Page::instance_count = 0;
int UIElement::ui_elements_instances = 0;

Renderer::Renderer(int w, int h):
W(w), H(h)
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

Page* Renderer::GetPage(int idx)
{
  if(idx < 0 || idx > pages.size()-1)
    return pages.back();
  else
    return pages[idx];
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

  Mat* page_img = nullptr;

  while(true)
  {
    if(pages.size() == 0 || pause == 1)
    {
      usleep((1.0/frame_rate) * 1000000);
      char c = cv::waitKey((1.0/frame_rate)*1000);
      if(int(c) != -1 && on_key_press != nullptr)
        (*on_key_press)(c);
      continue;
    }
    current_page = pages[page_idx];

    current_page->Draw();
    page_img = current_page->GetImage();
    DrawFooter(page_img);

    for(int i = 0; i < background.rows; i++)
    {
      for(int j = 0; j < background.cols; j++)
      {
        if(page_img->at<cv::Vec4b>(i, j)[3] == 0)
          render_image.at<cv::Vec4b>(i,j) = background.at<cv::Vec4b>(i,j);
        else
          render_image.at<cv::Vec4b>(i,j) = page_img->at<cv::Vec4b>(i,j);
      }
    }


    cv::imshow("Dashboard", render_image);
    // outputVideo << render_image;

    char c = cv::waitKey((1.0/frame_rate)*1000);
    if(c == -1)
      continue;
    if(current_page->command_mode){
      current_page->CommandMode(c);
    }else if (c == 13){
      current_page->command_mode = true;
    }else if(on_key_press != nullptr){
      (*on_key_press)(c);
    }
  }
}

void Renderer::DrawFooter(Mat* img)
{
  int page_indicator_w = W / pages.size();
  int page_indicator_h = H / 100;
  for(int i = 0; i < pages.size(); i++)
  {
    cv::rectangle(*img,
      Point((i  )*page_indicator_w, 0),
      Point((i+1)*page_indicator_w, page_indicator_h),

      Scalar(255, 255, 255, 255),
      page_idx == i ? -1 : 1
    );
  }
}


void Page::CommandMode(char c)
{
  if(ui_element_hovered != -1 && ui_elements[ui_element_hovered]->command_mode){
    ui_elements[ui_element_hovered]->CommandMode(c);
    return;
  }
  switch(c)
  {
    case 'a':
      if(ui_element_hovered != -1)
        ui_elements[ui_element_hovered]->hovered = false;
      ui_element_hovered --;
      if(ui_element_hovered < 0)
        ui_element_hovered = ui_elements.size()-1;
      ui_elements[ui_element_hovered]->hovered = true;
    break;
    case 'd':
      if(ui_element_hovered != -1)
        ui_elements[ui_element_hovered]->hovered = false;
      ui_element_hovered = (ui_element_hovered + 1) % ui_elements.size();
      ui_elements[ui_element_hovered]->hovered = true;
    break;
    case 13:
      if(ui_element_hovered != -1)
        ui_elements[ui_element_hovered]->command_mode = true;
    break;
    case 27:
      command_mode = false;
      if(ui_element_hovered != -1)
        ui_elements[ui_element_hovered]->hovered = false;
    break;
  }
}
