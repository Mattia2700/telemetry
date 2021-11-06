#include "Page4.h"

Page4::Page4(string name, int w, int h): Page(name, w, h)
{
  inv_l_graph = new Graph("Inveter L");
  inv_r_graph = new Graph("Inveter R");

  int margin = H/10;
  int graph_height = H/2 - margin*3/2;

  Box inv_l_box{margin, margin,
                W - 2 * margin, graph_height};
  Box inv_r_box{margin, graph_height + 2 * margin,
                W - 2 * margin, graph_height};

  inv_l_graph->SetPosition(inv_l_box);
  inv_r_graph->SetPosition(inv_r_box);

  inv_l_graph->SetLabels({"temp", "motor temp"});
  inv_r_graph->SetLabels({"temp", "motor temp"});

  ui_elements.push_back(inv_l_graph);
  ui_elements.push_back(inv_r_graph);
};

void Page4::Draw()
{
  if(current_data == nullptr)
    return;

  mtx.lock();

  background->setTo(background_color);

  ChimeraData* chim = (ChimeraData*)current_data;

  SetGraphData(chim);

  inv_l_graph->Draw(background);
  inv_r_graph->Draw(background);

  mtx.unlock();
  frame_count++;
}

void Page4::SetGraphData(ChimeraData* chim)
{

  int inverter_left_size = chim->data->inverter_left_size();
  vector<double> x(inverter_left_size, 0);
  vector<vector<double>> ys;

  ys.push_back(vector<double>(inverter_left_size, 0)); // temp
  ys.push_back(vector<double>(inverter_left_size, 0)); // motor_temp

  for(int i = 0; i < inverter_left_size; i++)
  {
    auto inverter_left = chim->data->inverter_left(i);
    x[i] = inverter_left.timestamp();

    ys[0][i] = inverter_left.temperature();
    ys[1][i] = inverter_left.motor_temp();
  }
  // throttle max is 100
  inv_l_graph->PushData(x, ys);
  inv_l_graph->SetMaxLenght(200);


  int inverter_right_size = chim->data->inverter_right_size();
  x.resize(inverter_right_size);
  ys.resize(2);
  ys[0].resize(inverter_right_size);
  ys[1].resize(inverter_right_size);

  for(int i = 0; i < inverter_right_size; i++)
  {
    auto inverter_right = chim->data->inverter_right(i);
    x[i] = inverter_right.timestamp();

    ys[0][i] = inverter_right.temperature();
    ys[1][i] = inverter_right.motor_temp();
  }
  inv_r_graph->PushData(x, ys);
  inv_r_graph->SetMaxLenght(200);
}
