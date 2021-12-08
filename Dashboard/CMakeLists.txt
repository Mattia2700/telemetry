project(Dashboard)

include_directories(
    inc
    inc/Pages
)

add_library(
    ${PROJECT_NAME}
    SHARED

    # src/PageManager.cpp
    src/Graph.cpp
    src/UIElement.cpp
    src/Application.cpp

    src/Page.cpp
    src/Pages/Page1.cpp

    # src/renderer.cpp
    # src/text_box.cpp
    # src/input_box.cpp
    # src/graph.cpp
    
    # src/Pages/Page1.cpp
    # src/Pages/Page2.cpp
    # src/Pages/Page3.cpp
    # src/Pages/Page4.cpp
    # src/Pages/Page5.cpp
    # src/Pages/Page6.cpp
    # src/Pages/Page7.cpp
    # src/Pages/Page8.cpp
    # src/Pages/Page9.cpp
    # src/Pages/Page10.cpp
    # src/Pages/Page11.cpp
)

target_link_libraries(${PROJECT_NAME}

    # STUFF
    ${Boost_LIBRARIES}
    ${Protobuf_LIBRARIES}
    websocketpp::websocketpp

    ${OpenCV_LIBS}

    # OPENGL
    glfw
    OpenGL::GL
    ${GLM_LIBRARIES}
    ${GLEW_LIBRARIES}
    libOpenGLCustom::OpenGLCustom

    libProtoDevices
)

add_executable(dashboard
  scripts/dashboard/dashboard.cpp
)
target_link_libraries(dashboard ${PROJECT_NAME})
