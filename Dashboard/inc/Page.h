#pragma once

#include <string>
#include <vector>
#include <mutex>


#include "Data.h"
#include "UIElement.h"

using namespace std;

class Page : public UIElement
{
public:
  Page(string name, int w, int h):UIElement(name),W(w),H(h), new_data(false)
  {
    instance_count ++;
  };
  ~Page()
  {
    instance_count --;
  }
  void SetData(Data* data)
  {
    unique_lock<mutex> lck(mtx);
    current_data = data;
    new_data = true;
  };


  virtual int Draw() = 0;

  virtual void OnEvent(Event& e){
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(OnMouseMove));
    dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(OnMouseScroll));
    dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonPressed));
    dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(OnMouseButtonReleased));
    dispatcher.Dispatch<KeyTypedEvent>(BIND_EVENT_FN(OnKeyTyped));
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnKeyPressed));
    dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(OnKeyReleased));

    for(auto ui_element : ui_elements)
    {
      ui_element->OnEvent(e);
      if(e.Handled)
        break;
    }
  };

  static int instance_count;
protected:
  Data* current_data = nullptr;
  bool new_data;

  int W, H;

  mutex mtx;

  vector<UIElement*> ui_elements;
  int ui_element_hovered = -1;
};
