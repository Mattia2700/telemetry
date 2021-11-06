#include "Page5.h"

Page5::Page5(string name, int w, int h): Page(name, w, h)
{
  throttle_graph = new Graph("Throttle");
  brake_graph  = new Graph("Brake");

  int margin = H/10;
  int graph_height = H/2 - margin*3/2;

  Box throttle_box{margin, margin,
                W - 2 * margin, graph_height};
  Box brake_box{margin, graph_height + 2 * margin,
                W - 2 * margin, graph_height};

  throttle_graph->SetPosition(throttle_box);
  brake_graph->SetPosition(brake_box);

  throttle_graph->SetLabels({"throttle1", "throttle2"});
  brake_graph->SetLabels({"brake_front", "brake_rear"});

  ui_elements.push_back(throttle_graph);
  ui_elements.push_back(brake_graph);
};

int Page5::Draw()
{
  if(current_data == nullptr || !new_data)
    return 0;

  mtx.lock();

  background->setTo(background_color);

  ChimeraData* chim = (ChimeraData*)current_data;

  SetGraphData(chim);

  throttle_graph->Draw(background);
  brake_graph->Draw(background);

  mtx.unlock();
  frame_count++;
  new_data = false;
  return 1;
}

void Page5::SetGraphData(ChimeraData* chim)
{
  int pedal_size = chim->data->pedal_size();
  if(pedal_size == 0)
    return;

  vector<double> x(pedal_size, 0);
  vector<vector<double>> ys1;
  vector<vector<double>> ys2;

  ys1.push_back(vector<double>(pedal_size,0)); // throttle1
  ys1.push_back(vector<double>(pedal_size,0)); // throttle2
  ys2.push_back(vector<double>(pedal_size,0)); // brake_front
  ys2.push_back(vector<double>(pedal_size,0)); // brake_rear

  for(int i = 0; i < pedal_size; i++)
  {
    auto pedal = chim->data->pedal(i);
    x[i] = pedal.timestamp();

    ys1[0][i] = pedal.throttle1();
    ys1[1][i] = pedal.throttle2();

    ys2[0][i] = pedal.brake_front();
    ys2[1][i] = pedal.brake_rear();
  }
  // throttle max is 100
  throttle_graph->SetScale(0, 100);
  throttle_graph->PushData(x, ys1);

  brake_graph->SetScale(0, 10);
  brake_graph->PushData(x, ys2);

  if(frame_count <= 2){
    throttle_graph->SetOffsetX(chim->data->pedal(0).timestamp());
    brake_graph->SetOffsetX(chim->data->pedal(0).timestamp());
  }
}
