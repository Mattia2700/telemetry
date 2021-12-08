#include "dashboard.h"

#include "Application.h"

#include "Page1.h"

float W = 1200;
float H = 800;

int main(int argc, char* argv[]) {

  Application app(W, H, "Dashboard");
  if(app.Init() < 0)
    return -1;

  Page1 page1("Page1", W, H);
  app.PushPage(&page1);


  
  app.Run();
  
}