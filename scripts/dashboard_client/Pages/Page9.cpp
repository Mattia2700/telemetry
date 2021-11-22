#include "Page9.h"

Page9::Page9(string name, int w, int h): Page(name, w, h)
{
  vadena.x1 = 46.43818912239108;
  vadena.y1 = 11.30368354228601;
  vadena.x2 = 46.43165275736119;
  vadena.y2 = 11.323467515385463;

  vadena_image = imread("vadena.png", IMREAD_UNCHANGED);

  cv::Size img_size = vadena_image.size();
};

int Page9::Draw()
{
  if(current_data == nullptr || !new_data)
    return 0;

  unique_lock<mutex> lck(mtx);

  // background->setTo(background_color);
  cv::resize(vadena_image, *background, background->size());

  ChimeraData* chim = (ChimeraData*)current_data;

  if(!SetGPSData(chim))
    return 0;


  double scaleX = double(W)/(vadena.x1 - vadena.x2);
  double scaleY = double(H)/(vadena.y2 - vadena.y1);

  if(latitude == 0)
    return 0;

  latitude = 46.435571712862384;
  longitude = 11.313650630685897;

  cv::circle(*background,
    Point((latitude-vadena.x2)*scaleX, (longitude-vadena.y1)*scaleY),
    8,
    Scalar(0, 0, 255, 255),
    -1
  );

  new_data = false;
  return 1;
}

int Page9::SetGPSData(ChimeraData* chim)
{
  int size = chim->data->gps1_size();
  if(size == 0)
    return 0;

  latitude = chim->data->gps1(size-1).latitude();
  longitude = chim->data->gps1(size-1).longitude();

  return 1;
}
