#pragma once

#include <string>

#include <libOpenGLCustom/Event.h>
#include <libOpenGLCustom/KeyEvent.h>
#include <libOpenGLCustom/ApplicationEvent.h>
#include <libOpenGLCustom/MouseEvent.h>

using namespace std;

struct Box
{
  float x;
  float y;
  float w;
  float h;
};

class UIElement
{
public:
  UIElement(string name="default"): name(name)
  {
    id = ui_elements_instances;
    ui_elements_instances ++;
  };

  int id;
  string name;
  bool command_mode = false;
  bool hovered = false;

  virtual void OnEvent(Event& e){
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(OnMouseMove));
    dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(OnMouseScroll));
    dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonPressed));
    dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(OnMouseButtonReleased));
    dispatcher.Dispatch<KeyTypedEvent>(BIND_EVENT_FN(OnKeyTyped));
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnKeyPressed));
    dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(OnKeyReleased));
  };

  // MOUSE
  virtual bool OnMouseMove(MouseMovedEvent&){return false;};
  virtual bool OnMouseScroll(MouseScrolledEvent&){return false;};
  virtual bool OnMouseButtonPressed(MouseButtonPressedEvent&){return false;};
  virtual bool OnMouseButtonReleased(MouseButtonReleasedEvent&){return false;};
  // KEYBOARD
  virtual bool OnKeyTyped(KeyTypedEvent&){return false;};
  virtual bool OnKeyPressed(KeyPressedEvent&){return false;};
  virtual bool OnKeyReleased(KeyReleasedEvent&){return false;};
private:
  static int ui_elements_instances;
};