#include "Page6.h"

Page6::Page6(string name, int w, int h): Page(name, w, h)
{
  steer_graph = new Graph("Angle");
  //brake_graph  = new Graph("Brake");

  int margin = H/10;
  int graph_height = H/2 - margin*3/2;

  Box throttle_box{margin, margin,
                W - 2 * margin, graph_height};
  //Box brake_box{margin, graph_height + 2 * margin,
  //              W - 2 * margin, graph_height};

  steer_graph->SetPosition(throttle_box);
  //brake_graph->SetPosition(brake_box);

  steer_graph->SetLabels({"angle"});
  //brake_graph->SetLabels({"brake_front", "brake_rear"});

  ui_elements.push_back(steer_graph);
  //ui_elements.push_back(brake_graph);
};

void Page6::Draw()
{
  if(current_data == nullptr)
    return;

  mtx.lock();

  background->setTo(background_color);

  ChimeraData* chim = (ChimeraData*)current_data;

  SetGraphData(chim);

  steer_graph->Draw(background);
  //brake_graph->Draw(background);

  mtx.unlock();
  frame_count++;
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
}
