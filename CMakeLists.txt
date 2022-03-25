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

###########
## BOOST ##
###########
find_package(Boost REQUIRED COMPONENTS
    iostreams system filesystem
)

#########################
## RaspiCam and OpenCV ##
#########################
find_package(raspicam PATHS /usr/local/lib/cmake/)
if(raspicam_FOUND)
  message(STATUS "____ Raspicam FOUND")
  find_package( OpenCV )
  if(OpenCV_FOUND)
    message(STATUS "____ OpenCV FOUND")
    set(WITH_CAMERA 1)
    add_definitions(-DWITH_CAMERA)
  endif()
endif()


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
  libBase
  SHARED

  src/can.cpp
  src/utils.cpp
  src/browse.cpp
  src/serial.cpp
  src/console.cpp

  src/wsclient.cpp
)
target_link_libraries(libBase
  stdc++fs
  websocketpp::websocketpp
)



add_library(
  libVehicle
  SHARED

  src/devices.cpp
  src/vehicle.cpp
  src/ubxparser.cpp
  src/gps_logger.cpp
  src/report.cpp

  # PROTO
  Protobuffer/cpp/devices.pb.cc
)
target_link_libraries(libVehicle
  hpdf
  stdc++fs
  ${Protobuf_LIBRARIES}
  Boost::iostreams
  Boost::system
  Boost::filesystem
)



add_library(
  libStateMachine
  SHARED
  
  # StateMachine
  src/StateMachine/StateMachine.cpp
  src/StateMachine/Fault.cpp
)
add_library(
  libSMImpl
  SHARED
  scripts/telemetry/telemetry_sm.cpp
)
target_link_libraries(libSMImpl
  stdc++fs
  ${Protobuf_LIBRARIES}
  websocketpp::websocketpp

  libBase
  libVehicle
  libJsonModels
  libStateMachine
)



add_library(
  libJsonModels
  SHARED

  # JSON
  json_models/session_config/json_loader.cpp
  json_models/telemetry_config/json_loader.cpp
  json_models/csv_parser_config/json_loader.cpp
  json_models/can_stat_json/json_loader.cpp
  json_models/gps_stat_json/json_loader.cpp
)

if(WITH_CAMERA)
  add_library(
    libCamera
    SHARED
    src/camera.cpp
  )
  target_link_libraries(libCamera
    libBase
    libStateMachine
    ${raspicam_LIBS}
    ${OpenCV_LIBS}
  )
  target_link_libraries(libSMImpl
  libCamera)
endif()




set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

## TELEMETRY
add_executable(telemetry scripts/telemetry/main.cpp)
target_link_libraries(telemetry libSMImpl libCamera)

## PORT SHARE
add_executable(share scripts/port/share.cpp src/serial.cpp)
add_executable(port_simulator scripts/port/port_simulator.cpp src/serial.cpp )
add_executable(read scripts/port/reader.cpp src/serial.cpp)

## CSV CONVERTER
add_executable(csv scripts/csv/csv.cpp)
target_link_libraries(csv
  libBase
  libVehicle
  libJsonModels
)

## LOG PLAYERS
add_executable(log_player scripts/log_player/log_player.cpp)
target_link_libraries(log_player libBase)
add_executable(gps_log_player scripts/log_player/gps_log_player.cpp)
target_link_libraries(gps_log_player libBase)