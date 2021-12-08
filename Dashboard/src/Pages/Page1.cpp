#include "Page1.h"

Page1::Page1(string name, int w, int h): Page(name, w, h)
{

  graph1 = new Graph("Graph1");
  graph2 = new Graph("Graph2");

  graph1->SetPosition({-1.0f, 0.0f,2.0f,1.0f});
  graph2->SetPosition({-1.0f,-1.0f,2.0f,1.0f});

  ys.push_back({});
  ys.push_back({});
  for(int i = 0; i < 1000; i++)
  {
    x.push_back(i);
    ys[0].push_back(sin(float(i)/(6.28*4)));
    ys[1].push_back(sin(float(i)/(6.28*8)));
  }
  graph1->SetLabels({"sine", "sine2"});
  graph2->SetLabels({"sine", "sine2"});

  ui_elements.push_back(graph1);
  ui_elements.push_back(graph2);
};

int Page1::Draw()
{
  graph1->PushData(x, ys);
  graph1->Draw();
  
  graph2->PushData(x, ys);
  graph2->Draw();

  return 1;
}

bool Page1::OnMouseMove(MouseMovedEvent& e)
{
  return false;
}