#include "Page3.h"

Page3::Page3(string name, int w, int h): Page(name, w, h)
{
  bms_hv_graph = new Graph("BMS HV");
  bms_lv_graph  = new Graph("BMS LV");

  int margin = H/10;
  int graph_height = H/2 - margin*3/2;

  Box bms_hv_box{margin, margin,
                W - 2 * margin, graph_height};
  Box bms_lv_box{margin, graph_height + 2 * margin,
                W - 2 * margin, graph_height};

  bms_hv_graph->SetPosition(bms_hv_box);
  bms_lv_graph->SetPosition(bms_lv_box);

  bms_hv_graph->SetLabels({
    "temp", "max temp", "min temp",
    "volt", "max volt", "min volt",
    "current", "power"});
  bms_lv_graph->SetLabels({"lv_temp", "lv_volt"});

  ui_elements.push_back(bms_hv_graph);
  ui_elements.push_back(bms_lv_graph);
};

int Page3::Draw()
{
  if(current_data == nullptr || !new_data)
    return 0;

  mtx.lock();

  background->setTo(background_color);

  ChimeraData* chim = (ChimeraData*)current_data;

  SetGraphData(chim);

  bms_hv_graph->Draw(background);
  bms_lv_graph->Draw(background);

  mtx.unlock();
  frame_count++;
  new_data = false;
  return 1;
}

void Page3::SetGraphData(ChimeraData* chim)
{

  int bms_hv_size = chim->data->bms_hv_size();

  vector<double> x(bms_hv_size, 0);
  vector<vector<double>> ys;

  ys.push_back(vector<double>(bms_hv_size, 0)); // hv_temp
  ys.push_back(vector<double>(bms_hv_size, 0)); // hv_max_temp
  ys.push_back(vector<double>(bms_hv_size, 0)); // hv_min_temp
  ys.push_back(vector<double>(bms_hv_size, 0)); // hv_volt
  ys.push_back(vector<double>(bms_hv_size, 0)); // hv_max_volt
  ys.push_back(vector<double>(bms_hv_size, 0)); // hv_min_volt
  ys.push_back(vector<double>(bms_hv_size, 0)); // hv_current
  ys.push_back(vector<double>(bms_hv_size, 0)); // hv_power

  for(int i = 0; i < bms_hv_size; i++)
  {
    auto bms_hv = chim->data->bms_hv(i);
    x[i] = bms_hv.timestamp();

    ys[0][i] = bms_hv.temperature();
    ys[1][i] = bms_hv.max_temperature();
    ys[2][i] = bms_hv.min_temperature();
    ys[3][i] = bms_hv.voltage();
    ys[4][i] = bms_hv.max_voltage();
    ys[5][i] = bms_hv.min_voltage();
    ys[6][i] = bms_hv.current();
    ys[7][i] = bms_hv.power();
  }
  bms_hv_graph->PushData(x, ys);
  bms_hv_graph->SetMaxLenght(200);

  int bms_lv_size = chim->data->bms_lv_size();
  x.resize(bms_lv_size);
  ys.resize(2);
  ys[0].resize(bms_lv_size); // lv_temp
  ys[1].resize(bms_lv_size); // lv_volt

  for(int i = 0; i < bms_lv_size; i++)
  {
    auto bms_lv = chim->data->bms_lv(i);
    x[i] = bms_lv.timestamp();

    ys[0][i] = bms_lv.temperature();
    ys[1][i] = bms_lv.voltage();
  }
  bms_lv_graph->PushData(x, ys);
  bms_lv_graph->SetMaxLenght(100);
}
