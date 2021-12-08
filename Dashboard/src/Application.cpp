#include "Application.h"

Application* Application::m_SelfInstance = nullptr;

Application::Application(float W, float H, std::string name)
{
  if(m_SelfInstance)
    throw logic_error("Application Already exists");
  m_SelfInstance = this;
  
  m_ApplicationWindow.W = W;
  m_ApplicationWindow.H = H;

  m_ApplicationWindow.name = name;

  m_ApplicationWindow.gl_window = nullptr;
}

int Application::Init()
{
    if (!glfwInit())
      return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    m_ApplicationWindow.gl_window = glfwCreateWindow( m_ApplicationWindow.W,
                                                      m_ApplicationWindow.H,
                                                      m_ApplicationWindow.name.c_str(),
                                                      NULL,
                                                      NULL
                                                      );
    if (!m_ApplicationWindow.gl_window)
    {
      glfwTerminate();
      return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(m_ApplicationWindow.gl_window);

    if (glewInit() != GLEW_OK)
    {
      std::cout << "glew init failed" << std::endl;
      return -1;
    }

    glfwSetWindowUserPointer(m_ApplicationWindow.gl_window, &m_ApplicationWindow);
    SetGLFWCallbacks();

    m_ApplicationWindow.Callback = BIND_EVENT_FN(Application::OnEvent);



    return 1;
}

void Application::Run()
{
  RendererAPI::Init();

  RendererAPI::SetViewport(0, 0, m_ApplicationWindow.W, m_ApplicationWindow.H);

  Renderer::Init();

  RendererAPI::SetLineWidth(10.0f);
  RendererAPI::SetClearColor(glm::vec4{0.09f,0.09f,0.11f,1.0f});


  while(!glfwWindowShouldClose(m_ApplicationWindow.gl_window))
  {
    RendererAPI::Clear();

    Renderer::BeginScene();

    if(m_Pages.size() > 0)
    {
      m_Pages[m_PageIndex]->Draw();
    }

    Renderer::EndScene();
    Renderer::Flush();

    glfwSwapBuffers(m_ApplicationWindow.gl_window);
    glfwPollEvents();
  }
}

void Application::OnEvent(Event& e){
  EventDispatcher dispatcher(e);

  dispatcher.Dispatch<KeyTypedEvent>(BIND_EVENT_FN(Application::OnKeyTyped));


  if(m_Pages.size() >= 0)
    m_Pages[m_PageIndex]->OnEvent(e);
}

bool Application::OnKeyTyped(KeyTypedEvent& e){
  switch (e.GetKeyCode())
  {
  case 'a':
      if(m_Pages.size() == 0)
        break;
      m_PageIndex --;
      if(m_PageIndex < 0)
        m_PageIndex = m_Pages.size()-1;
    break;
  case 'd':
      if(m_Pages.size() == 0)
        break;
      m_PageIndex = (m_PageIndex + 1) % m_Pages.size();
    break;
  case 'w':
    break;
  case 's':
    break;
  default:
    break;
  }

  return false;
}

void Application::SetGLFWCallbacks()
{
	glfwSetWindowSizeCallback(m_ApplicationWindow.gl_window, [](GLFWwindow* window, int width, int height)
	{
		ApplicationWindow wnd = *(ApplicationWindow*)glfwGetWindowUserPointer(window);
		// RendererAPI::SetViewport(0, 0, width, height);

		if(wnd.Callback)
		{
			WindowResizeEvent event(width, height);
			wnd.Callback(event);
		}
	});

	glfwSetWindowCloseCallback(m_ApplicationWindow.gl_window, [](GLFWwindow* window)
	{
		ApplicationWindow& data = *(ApplicationWindow*)glfwGetWindowUserPointer(window);
		WindowCloseEvent event;
		data.Callback(event);
	});

	glfwSetCursorPosCallback(m_ApplicationWindow.gl_window, [](GLFWwindow* window, double posx, double posy)
	{
		ApplicationWindow wnd = *(ApplicationWindow*)glfwGetWindowUserPointer(window);
		if(wnd.Callback)
		{
			MouseMovedEvent event(posx, posy);
			wnd.Callback(event);
		}
	});

	glfwSetKeyCallback(m_ApplicationWindow.gl_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		ApplicationWindow& data = *(ApplicationWindow*)glfwGetWindowUserPointer(window);

		switch (action)
		{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.Callback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.Callback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.Callback(event);
				break;
			}
		}
	});

	glfwSetCharCallback(m_ApplicationWindow.gl_window, [](GLFWwindow* window, unsigned int keycode)
	{
		ApplicationWindow& data = *(ApplicationWindow*)glfwGetWindowUserPointer(window);

		KeyTypedEvent event(keycode);
		data.Callback(event);
	});

	glfwSetMouseButtonCallback(m_ApplicationWindow.gl_window, [](GLFWwindow* window, int button, int action, int mods)
	{
		ApplicationWindow& data = *(ApplicationWindow*)glfwGetWindowUserPointer(window);

		switch (action)
		{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.Callback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.Callback(event);
				break;
			}
		}
	});

	glfwSetScrollCallback(m_ApplicationWindow.gl_window, [](GLFWwindow* window, double xOffset, double yOffset)
	{
		ApplicationWindow& data = *(ApplicationWindow*)glfwGetWindowUserPointer(window);

		MouseScrolledEvent event((float)xOffset, (float)yOffset);
		data.Callback(event);
	});

	glfwSetCursorPosCallback(m_ApplicationWindow.gl_window, [](GLFWwindow* window, double xPos, double yPos)
	{
		ApplicationWindow& data = *(ApplicationWindow*)glfwGetWindowUserPointer(window);

		MouseMovedEvent event((float)xPos, (float)yPos);
		data.Callback(event);
	});
}