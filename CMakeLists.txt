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
    message(STATUS "____ Protobuf FOUND")
    message(STATUS "Using Protocol Buffers ${Protobuf_VERSION}")
    include_directories(${PROTOBUF_INCLUDE_DIR})
endif()

###############
## WEBSOCKET ##
###############
find_package(websocketpp REQUIRED)
if(websocketpp_FOUND)
  message(STATUS "____ Websocketpp FOUND")
endif()

find_package(Boost REQUIRED COMPONENTS
    iostreams system filesystem
)



include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/inc
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
  ${CMAKE_CURRENT_SOURCE_DIR}/json_models
  ${CMAKE_CURRENT_SOURCE_DIR}/Protobuffer/cpp
  
  # haru
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libharu
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libharu/include

  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gnuplot-iostream
  )
#############
## LIBHARU ##
#############
# add_definitions(-DLIBHPDF_DEBUG_TRACE)
# add_definitions(-DHPDF_PTRACE_ON)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/libharu)

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
  src/gps_logger.cpp
  src/report.cpp

  # PROTO
  Protobuffer/cpp/devices.pb.cc

  # JSON
  json_models/session_config/json_loader.cpp
  json_models/telemetry_config/json_loader.cpp
  json_models/csv_parser_config/json_loader.cpp
  json_models/can_stat_json/json_loader.cpp
  json_models/gps_stat_json/json_loader.cpp

  src/wsclient.cpp
  )
target_link_libraries(${PROJECT_NAME}
  hpdf
  stdc++fs
  ${Protobuf_LIBRARIES}
  websocketpp::websocketpp
  Boost::iostreams
  Boost::system
  Boost::filesystem
  )



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

## TELEMETRY
add_executable(telemetry scripts/telemetry/telemetry.cpp)
target_link_libraries(telemetry ${PROJECT_NAME})

## PORT SHARE
add_executable(share scripts/port/share.cpp src/serial.cpp)
add_executable(port_simulator scripts/port/port_simulator.cpp src/serial.cpp )
add_executable(read scripts/port/reader.cpp src/serial.cpp)

## CSV CONVERTER
add_executable(csv scripts/csv/csv.cpp)
target_link_libraries(csv
  ${Protobuf_LIBRARIES}
  ${PROJECT_NAME}
  Boost::iostreams
  Boost::system
  Boost::filesystem)

## LOG PLAYERS
add_executable(log_player scripts/log_player/log_player.cpp)
target_link_libraries(log_player ${PROJECT_NAME})
add_executable(gps_log_player scripts/log_player/gps_log_player.cpp)
target_link_libraries(gps_log_player ${PROJECT_NAME})

## DASHBOARD
add_executable(dashboard_simulator scripts/dashboard/dashboard.cpp)
target_link_libraries(dashboard_simulator ${PROJECT_NAME})