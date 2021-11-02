#include "Page2.h"

Page2::Page2(string name, int w, int h): Page(name, w, h)
{
  accel_graph = new Graph();
  gyro_graph  = new Graph();

  int margin = H/10;
  int graph_height = H/2 - margin*3/2;

  Box accel_box{margin, margin,
                W - 2 * margin, graph_height};
  Box  gyro_box{margin, graph_height + 2 * margin,
                W - 2 * margin, graph_height};

  accel_graph->SetPosition(accel_box);
  gyro_graph->SetPosition(gyro_box);
};

void Page2::Draw()
{
  if(current_data == nullptr)
    return;

  mtx.lock();

  background->setTo(background_color);

  ChimeraData* chim = (ChimeraData*)current_data;

  SetAccelGyroData(chim);

  accel_graph->Draw(background);
  gyro_graph->Draw(background);

  mtx.unlock();
  frame_count++;
}

void Page2::SetAccelGyroData(ChimeraData* chim)
{
  int accel_size = chim->data->accel_size();
  if(accel_size == 0)
    return;

  vector<double> x(accel_size, 0);
  vector<vector<double>> ys;

  ys.push_back(vector<double>(accel_size, 0));
  ys.push_back(vector<double>(accel_size, 0));
  ys.push_back(vector<double>(accel_size, 0));

  for(int i = 0; i < accel_size; i++)
  {
    auto accel = chim->data->accel(i);
    x[i] = accel.timestamp();

    ys[0][i] = accel.x();
    ys[1][i] = accel.y();
    ys[2][i] = accel.z();
  }
  accel_graph->SetScale(0, chim->data->accel(0).scale());
  accel_graph->PushData(x, ys);
  if(frame_count == 2)
    accel_graph->SetOffsetX(chim->data->accel(0).timestamp());


  int gyro_size = chim->data->gyro_size();
  if(gyro_size == 0)
    return;

  try{
    x.resize(gyro_size);
  }catch(exception e)
  {
    cout << e.what() << endl;
    return;
  }
  ys[0].resize(gyro_size);
  ys[1].resize(gyro_size);
  ys[2].resize(gyro_size);

  for(int i = 0; i < gyro_size; i++)
  {
    auto gyro = chim->data->gyro(i);
    x[i] = gyro.timestamp();

    ys[0][i] = gyro.x();
    ys[1][i] = gyro.y();
    ys[2][i] = gyro.z();
  }
  gyro_graph->SetScale(0, chim->data->gyro(0).scale());
  gyro_graph->PushData(x, ys);
  if(frame_count == 2)
    gyro_graph->SetOffsetX(chim->data->gyro(0).timestamp());
}
