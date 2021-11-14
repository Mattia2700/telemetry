#include "Page1.h"

Page1::Page1(string name, int w, int h): Page(name, w, h)
{
  pedal_graph = new Graph("Pedal");
  speed_graph  = new Graph("Speed");

  int margin = H/10;
  int graph_height = H/2 - margin*3/2;

  Box pedal_box{margin, margin,
                W - 2 * margin, graph_height};
  Box speed_box{margin, graph_height + 2 * margin,
                W - 2 * margin, graph_height};

  pedal_graph->SetPosition(pedal_box);
  speed_graph->SetPosition(speed_box);

  pedal_graph->SetLabels({"throttle", "brake front"});
  speed_graph->SetLabels({"rads_left", "rads_right"});

  ui_elements.push_back(pedal_graph);
  ui_elements.push_back(speed_graph);
};

int Page1::Draw()
{
  if(current_data == nullptr || !new_data)
    return 0;

  mtx.lock();

  background->setTo(background_color);

  ChimeraData* chim = (ChimeraData*)current_data;

  SetGraphData(chim);

  pedal_graph->Draw(background);
  speed_graph->Draw(background);

  mtx.unlock();
  frame_count++;
  new_data = false;
  return 1;
}

void Page1::SetGraphData(ChimeraData* chim)
{
  // vector<vector<double>> ys = {
  //   chim->sensor_values["pedal"]["throttle1"],
  //   chim->sensor_values["pedal"]["brake_front"]
  // };
  // pedal_graph->PushData(chim->sensor_values["pedal"]["timestamp"], ys);


  int pedal_size = chim->data->pedal_size();
  if(pedal_size == 0)
    return;

  vector<double> x(pedal_size, 0);
  vector<vector<double>> ys;

  ys.push_back(vector<double>(pedal_size,0)); // throttle1
  ys.push_back(vector<double>(pedal_size,0)); // bake front

  for(int i = 0; i < pedal_size; i++)
  {
    auto pedal = chim->data->pedal(i);
    x[i] = pedal.timestamp();

    ys[0][i] = pedal.throttle1();
    ys[1][i] = pedal.brake_front();
  }
  pedal_graph->PushData(x, ys);


  int speed_left_size = chim->data->encoder_left_size();
  int speed_right_size = chim->data->encoder_right_size();
  int max_size = speed_left_size > speed_right_size ? speed_left_size : speed_right_size;
  if(max_size == 0)
    return;

  x.resize(max_size);
  ys[0].resize(speed_left_size); // speed left
  ys[1].resize(speed_left_size); // speed right

  for(int i = 0; i < max_size; i++)
  {
    if(i < chim->data->encoder_left_size())
    {
      auto speed_l = chim->data->encoder_left(i);
      x[i] = speed_l.timestamp();
      ys[0][i] = speed_l.rads();
    }
    if(i < chim->data->encoder_right_size())
    {
      auto speed_r = chim->data->encoder_right(i);
      ys[1][i] = speed_r.rads();
    }

  }
  speed_graph->PushData(x, ys);
}
