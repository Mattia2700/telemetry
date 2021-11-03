#include "Page3.h"

Page3::Page3(string name, int w, int h): Page(name, w, h)
{
  bms_graph = new Graph("BMS");
  inv_graph  = new Graph("Inveter");

  int margin = H/10;
  int graph_height = H/2 - margin*3/2;

  Box bms_box{margin, margin,
                W - 2 * margin, graph_height};
  Box inv_box{margin, graph_height + 2 * margin,
                W - 2 * margin, graph_height};

  bms_graph->SetPosition(bms_box);
  inv_graph->SetPosition(inv_box);

  bms_graph->SetLabels({"lv_temp", "hv_temp", "lv_volt", "hv_volt", "hv_current"});

  inv_graph->SetLabels({"right_temp", "left_temp", "right_motor_temp", "left_motor_temp"});
  ui_elements.push_back(bms_graph);
  ui_elements.push_back(inv_graph);
};

void Page3::Draw()
{
  if(current_data == nullptr)
    return;

  mtx.lock();

  background->setTo(background_color);

  ChimeraData* chim = (ChimeraData*)current_data;

  SetGraphData(chim);

  bms_graph->Draw(background);
  inv_graph->Draw(background);

  mtx.unlock();
  frame_count++;
}

void Page3::SetGraphData(ChimeraData* chim)
{

  int bms_lv_size = chim->data->bms_lv_size();
  int bms_hv_size = chim->data->bms_hv_size();
  int max_size = bms_lv_size > bms_hv_size ? bms_lv_size : bms_hv_size;
  if(max_size == 0)
    return;

  {
    vector<double> x(max_size, 0);
    vector<vector<double>> ys;

    ys.push_back(vector<double>(bms_lv_size, 0)); // lv_temp
    ys.push_back(vector<double>(bms_hv_size, 0)); // hv_temp
    ys.push_back(vector<double>(bms_lv_size, 0)); // lv_volt
    ys.push_back(vector<double>(bms_hv_size, 0)); // hv_volt
    ys.push_back(vector<double>(bms_hv_size, 0)); // hv_current


    for(int i = 0; i < max_size; i++)
    {
      if(i < chim->data->bms_lv_size())
      {
        auto bms_lv = chim->data->bms_lv(i);
        ys[0][i] = bms_lv.temperature();
        ys[2][i] = bms_lv.voltage();

        if(max_size == bms_lv_size)
          x[i] = bms_lv.timestamp();
      }
      if(i < chim->data->bms_hv_size())
      {
        auto bms_hv = chim->data->bms_hv(i);
        ys[1][i] = bms_hv.temperature();
        ys[3][i] = bms_hv.voltage();
        ys[4][i] = bms_hv.current();

        if(max_size == bms_hv_size)
          x[i] = bms_hv.timestamp();
      }
    }
    // throttle max is 100
    bms_graph->SetScale(0, 50);
    bms_graph->PushData(x, ys);
    bms_graph->SetMaxLenght(200);
    if(frame_count <= 2){
      if(chim->data->bms_hv_size() > 0)
        bms_graph->SetOffsetX(chim->data->bms_hv(0).timestamp());
      else if(chim->data->bms_lv_size() > 0)
        bms_graph->SetOffsetX(chim->data->bms_lv(0).timestamp());
    }
  }


  int inverter_left_size = chim->data->inverter_left_size();
  int inverter_right_size = chim->data->inverter_right_size();
  max_size = inverter_left_size > inverter_right_size ? inverter_left_size : inverter_right_size;
  if(max_size == 0)
    return;

  {
    vector<double> x(max_size, 0);
    vector<vector<double>> ys;
    ys.push_back(vector<double>(inverter_left_size, 0)); // inv_l_temp
    ys.push_back(vector<double>(inverter_right_size, 0)); // inv_r_temp
    ys.push_back(vector<double>(inverter_left_size, 0)); // inv_l_motor_temp
    ys.push_back(vector<double>(inverter_right_size, 0)); // inv_r_motor_temp


    for(int i = 0; i < max_size; i++)
    {
      if(i < chim->data->inverter_left_size())
      {
        auto inverter_left = chim->data->inverter_left(i);
        ys[0][i] = inverter_left.temperature();
        ys[2][i] = inverter_left.motor_temp();

        if(max_size == inverter_left_size)
          x[i] = inverter_left.timestamp();
      }
      if(i < chim->data->inverter_right_size())
      {
        auto inverter_right = chim->data->inverter_right(i);
        ys[1][i] = inverter_right.temperature();
        ys[3][i] = inverter_right.motor_temp();

        if(max_size == inverter_right_size)
          x[i] = inverter_right.timestamp();
      }
    }
    // throttle max is 100
    inv_graph->SetScale(0, 50);
    inv_graph->PushData(x, ys);
    inv_graph->SetMaxLenght(200);
    if(frame_count <= 2){
      if(chim->data->inverter_right_size() > 0)
        inv_graph->SetOffsetX(chim->data->inverter_right(0).timestamp());
      else if(chim->data->inverter_left_size() > 0)
        inv_graph->SetOffsetX(chim->data->inverter_left(0).timestamp());
    }
  }
}
