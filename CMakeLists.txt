cmake_minimum_required(VERSION 3.16)

project(Telemetry)

SET(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_CXX_STANDARD_REQUIRED True)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread -g")

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


include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/inc
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
  ${CMAKE_CURRENT_SOURCE_DIR}/json_models
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
  # SRC
  src/can.cpp
  src/utils.cpp
  src/browse.cpp
  src/serial.cpp
  src/devices.cpp
  src/vehicle.cpp
  src/console.cpp

  # PROTO
  Protobuffer/cpp/devices.pb.cc

  # JSON
  json_models/telemetry_config/json_loader.cpp
  json_models/session_config/json_loader.cpp

  src/wsclient.cpp
  )
target_link_libraries(${PROJECT_NAME} stdc++fs)

if(websocketpp_FOUND)
  target_link_libraries(${PROJECT_NAME} websocketpp::websocketpp)
endif()

if(Protobuf_FOUND)
  target_link_libraries(${PROJECT_NAME} ${Protobuf_LIBRARIES})
endif()




set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(telemetry scripts/telemetry/telemetry.cpp scripts/telemetry/gps_logger.cpp)
target_link_libraries(telemetry ${Protobuf_LIBRARIES} ${PROJECT_NAME} websocketpp::websocketpp)

add_executable(csv scripts/csv/csv.cpp)
target_link_libraries(csv ${Protobuf_LIBRARIES} ${PROJECT_NAME})

add_executable(log_player scripts/log_player/log_player.cpp)
target_link_libraries(log_player ${PROJECT_NAME})
add_executable(gps_log_player scripts/log_player/gps_log_player.cpp)
target_link_libraries(gps_log_player ${PROJECT_NAME})


find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

add_executable(share scripts/port/share.cpp src/serial.cpp)
add_executable(port_simulator scripts/port/port_simulator.cpp src/serial.cpp )
add_executable(read scripts/port/reader.cpp src/serial.cpp)


if(websocketpp_FOUND AND Protobuf_FOUND)
  add_executable(dashboard_simulator scripts/dashboard/dashboard.cpp)
  target_link_libraries(dashboard_simulator ${Protobuf_LIBRARIES} websocketpp::websocketpp ${PROJECT_NAME})
endif()
