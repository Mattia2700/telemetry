#include "Page1.h"

Page1::Page1(string name, int w, int h): Page(name, w, h)
{

  pedal_graph = new Graph("Pedal");
  speed_graph = new Graph("Speed");

  float margin = 0.04f;
  float graph_height = 1.0f - margin*3/2;
  Box pedal_box{-1.0f + margin, -1.0f + margin,            2.0f - 2 * margin, graph_height};
  Box speed_box{-1.0f + margin, graph_height + 2 * margin - 1.0f, 2.0f - 2 * margin, graph_height};

  pedal_graph->SetPosition(pedal_box);
  speed_graph->SetPosition(speed_box);


  pedal_graph->SetLabels({"throttle", "brake front"});
  speed_graph->SetLabels({"rads_left", "rads_right"});

  pedal_graph->SetMaxLenght(10000);
  speed_graph->SetMaxLenght(10000);

  ui_elements.push_back(pedal_graph);
  ui_elements.push_back(speed_graph);
};

int Page1::Draw()
{
  ChimeraData* chim = (ChimeraData*)current_data;
  
  chim->newData = false;

  vector<double> x;
  vector<vector<double>> ys;

  {
    ys.resize(2);
    std::copy(chim->data->pedal.timestamp.begin(), chim->data->pedal.timestamp.end(), std::back_inserter(x));
    std::copy(chim->data->pedal.throttle1.begin(), chim->data->pedal.throttle1.end(), std::back_inserter(ys[0]));
    std::copy(chim->data->pedal.brake_front.begin(), chim->data->pedal.brake_front.end(), std::back_inserter(ys[1]));
    pedal_graph->PushData(x, ys);
    pedal_graph->Draw();
  }
  ys.clear();
  {
    ys.resize(2);
    std::copy(chim->data->encoder_left.timestamp.begin(), chim->data->encoder_left.timestamp.end(), std::back_inserter(x));
    std::copy(chim->data->encoder_left.rads.begin(), chim->data->encoder_left.rads.end(), std::back_inserter(ys[0]));
    std::copy(chim->data->encoder_right.rads.begin(), chim->data->encoder_right.rads.end(), std::back_inserter(ys[1]));
    speed_graph->PushData(x, ys);
    speed_graph->Draw();
  }

  return 1;
}

bool Page1::OnMouseMove(MouseMovedEvent& e)
{
  return false;
}