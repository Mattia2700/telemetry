#include "Page6.h"

Page6::Page6(string name, int w, int h): Page(name, w, h)
{
  steer_graph = new Graph("Steer");
  gyro_graph  = new Graph("Gyro");

  int margin = H/10;
  int graph_height = H/2 - margin*3/2;

  Box throttle_box{margin, margin,
                W - 2 * margin, graph_height};
  Box gyro_box{margin, graph_height + 2 * margin,
               W - 2 * margin, graph_height};

  steer_graph->SetPosition(throttle_box);
  gyro_graph->SetPosition(gyro_box);

  steer_graph->SetLabels({"angle"});
  gyro_graph->SetLabels({"z"});

  ui_elements.push_back(steer_graph);
  ui_elements.push_back(gyro_graph);
};

int Page6::Draw()
{
  if(current_data == nullptr || !new_data)
    return 0;

  mtx.lock();

  background->setTo(background_color);

  ChimeraData* chim = (ChimeraData*)current_data;

  SetGraphData(chim);

  steer_graph->Draw(background);
  gyro_graph->Draw(background);

  mtx.unlock();
  frame_count++;
  new_data = false;
  return 1;
}

void Page6::SetGraphData(ChimeraData* chim)
{
  int steer_size = chim->data->steer_size();
  if(steer_size == 0)
    return;

  vector<double> x(steer_size, 0);
  vector<vector<double>> ys;

  ys.push_back(vector<double>(steer_size,0)); // throttle1

  for(int i = 0; i < steer_size; i++)
  {
    auto steer = chim->data->steer(i);
    x[i] = steer.timestamp();

    ys[0][i] = steer.angle();
  }
  steer_graph->PushData(x, ys);


  int gyro_size = chim->data->gyro_size();
  if(gyro_size == 0)
    return;


  x.resize(gyro_size);
  ys[0].resize(gyro_size);

  for(int i = 0; i < gyro_size; i++)
  {
    auto gyro = chim->data->gyro(i);
    x[i] = gyro.timestamp();

    ys[0][i] = gyro.z();
  }
  gyro_graph->PushData(x, ys);
}
