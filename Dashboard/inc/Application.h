#pragma once

#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <libOpenGLCustom/Renderer.h>
#include <libOpenGLCustom/RendererAPI.h>
#include <libOpenGLCustom/Event.h>
#include <libOpenGLCustom/KeyEvent.h>
#include <libOpenGLCustom/MouseEvent.h>
#include <libOpenGLCustom/ApplicationEvent.h>

#include "Page.h"

struct ApplicationWindow
{
  float W;
  float H;

  std::string name;

  EventCallbackFn Callback;
  GLFWwindow* gl_window;
};

class Application
{
public:

  Application(float W, float H, std::string name="Application");
  int Init();

  void PushPage(Page* page)
  {
    m_Pages.push_back(page);
  }

  void Run();

	static Application& Get(){return *m_SelfInstance;};
	GLFWwindow* GetGLWindow(){return m_ApplicationWindow.gl_window;};
	const ApplicationWindow* GetWindow(){return &m_ApplicationWindow;};
private:
  ApplicationWindow m_ApplicationWindow;

  vector<Page *> m_Pages;

  int m_PageIndex = 0;

  void SetGLFWCallbacks();
  void OnEvent(Event &e);
  
  bool OnKeyTyped(KeyTypedEvent&);

	static Application* m_SelfInstance;
};