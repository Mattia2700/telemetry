cmake_minimum_required(VERSION 3.10)

project(Telemetry)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread -g")

###########
## BOOST ##
###########
find_package(Boost  1.54.0
   REQUIRED
   COMPONENTS serialization filesystem
)
if(Boost_FOUND)
  message(STATUS "Boost FOUND")
endif()


###########
## PROTO ##
###########
find_package(Protobuf REQUIRED)
if(Protobuf_FOUND)
    message(STATUS "Using Protocol Buffers ${Protobuf_VERSION}")
    include_directories(${PROTOBUF_INCLUDE_DIR})
endif()

###############
## WEBSOCKET ##
###############
find_package(websocketpp)
if(websocketpp_FOUND)
  message(STATUS "Websocketpp FOUND")
endif()


############
## OPENCV ##
############
find_package(OpenCV)
if(OpenCV_FOUND)
  message(STATUS "OpenCV FOUND")
  add_compile_definitions(WITH_OPENCV)
endif()


include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/inc
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
  ${CMAKE_CURRENT_SOURCE_DIR}/Protobuffer/cpp

  ${CURL_INCLUDE_DIR}
  )


if(PythonLibs_FOUND)
  include_directories(${PYTHON_INCLUDE_DIRS})
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(
  ${PROJECT_NAME}
  SHARED
  src/can.cpp
  src/utils.cpp
  src/browse.cpp
  src/serial.cpp
  src/devices.cpp
  src/vehicle.cpp
  src/console.cpp
  Protobuffer/cpp/devices.pb.cc
  )

if(websocketpp_FOUND)
  target_link_libraries(${PROJECT_NAME} websocketpp::websocketpp)
endif()

if(Protobuf_FOUND)
  target_link_libraries(${PROJECT_NAME} ${Protobuf_LIBRARIES})
endif()

if(Boost_FOUND)
  target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()

if(OpenCV_FOUND)
  target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(logger scripts/logger/logger.cpp)
target_link_libraries(logger ${Protobuf_LIBRARIES} ${Boost_LIBRARIES} ${PROJECT_NAME})

add_executable(telemetry scripts/telemetry/telemetry.cpp scripts/telemetry/gps_logger.cpp)
target_link_libraries(telemetry ${Protobuf_LIBRARIES} ${Boost_LIBRARIES} ${PROJECT_NAME} websocketpp::websocketpp)

add_executable(csv scripts/csv/csv.cpp)
target_link_libraries(csv ${Protobuf_LIBRARIES} ${Boost_LIBRARIES} ${PROJECT_NAME})

add_executable(log_trimmer scripts/log_trimmer/log_trimmer.cpp)
target_link_libraries(log_trimmer ${Protobuf_LIBRARIES} ${Boost_LIBRARIES} ${PROJECT_NAME})


add_executable(share scripts/port/share.cpp src/serial.cpp)
add_executable(port_simulator scripts/port/port_simulator.cpp src/serial.cpp)
add_executable(read scripts/port/reader.cpp src/serial.cpp)


if(websocketpp_FOUND AND Protobuf_FOUND AND Boost_FOUND)
  add_executable(dashboard scripts/dashboard/dashboard.cpp)
  target_link_libraries(dashboard ${Protobuf_LIBRARIES} ${Boost_LIBRARIES} websocketpp::websocketpp ${PROJECT_NAME})

  add_executable(dashboard_can scripts/dashboard_can/dashboard_can.cpp)
  target_link_libraries(dashboard_can ${Protobuf_LIBRARIES} ${Boost_LIBRARIES} websocketpp::websocketpp ${PROJECT_NAME})

  if(OpenCV_FOUND)
    add_executable(dashboard_client
      scripts/dashboard_client/dashboard_client.cpp

      src/renderer.cpp
      src/text_box.cpp
      src/input_box.cpp
      src/graph.cpp

      scripts/dashboard_client/Pages/Page1.cpp
      scripts/dashboard_client/Pages/Page2.cpp
      scripts/dashboard_client/Pages/Page3.cpp
      scripts/dashboard_client/Pages/Page4.cpp
      scripts/dashboard_client/Pages/Page5.cpp
      scripts/dashboard_client/Pages/Page6.cpp
      scripts/dashboard_client/Pages/Page7.cpp
      scripts/dashboard_client/Pages/Page8.cpp
      scripts/dashboard_client/Pages/Page9.cpp
      scripts/dashboard_client/Pages/Page10.cpp
      scripts/dashboard_client/Pages/Page11.cpp
    )
    target_link_libraries(dashboard_client
      ${Protobuf_LIBRARIES}
      ${Boost_LIBRARIES}
      websocketpp::websocketpp
      ${OpenCV_LIBS}
      ${PROJECT_NAME}
    )
  endif()
endif()
