cmake_minimum_required(VERSION 3.16)

project(Telemetry)

SET(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_CXX_STANDARD_REQUIRED True)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread -O3")

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
    iostreams system
)

#########
## ZMQ ##
#########
find_package(PkgConfig)
pkg_check_modules(PC_ZeroMQ QUIET zmq)

find_path(ZeroMQ_INCLUDE_DIR
   NAMES zmq.hpp
   PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
)
find_library(ZeroMQ_LIBRARY
   NAMES zmq
   PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
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
  ${PROJECT_SOURCE_DIR}/thirdparty/libharu
  ${PROJECT_SOURCE_DIR}/thirdparty/libharu/include

  # Gnuplot
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/gnuplot-iostream

  # Lap Counter
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lapcounter

  # Can generated structs
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/can
  )
#############
## LIBHARU ##
#############
# add_definitions(-DLIBHPDF_DEBUG_TRACE)
# add_definitions(-DHPDF_PTRACE_ON)
add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/libharu)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

FILE(GLOB srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/*)

add_library(
  canlib
  SHARED
  src/implemetations.cpp
)
target_link_libraries(canlib ${Protobuf_LIBRARIES})

add_library(
  libBase
  SHARED

  # ${srcs}
  src/browse.cpp
  src/camera.cpp
  src/inverter.cpp
  src/can.cpp
  src/gps.cpp
  src/connection.cpp
  src/gps_logger.cpp
  src/loads.cpp
  src/serial.cpp
  src/utils.cpp
  src/ws_connection.cpp
  src/zmq_connection.cpp
  src/console.cpp

  # StateMachine
  src/StateMachine/StateMachine.cpp
  src/StateMachine/Fault.cpp

  scripts/telemetry/telemetry_sm.cpp
 
  thirdparty/can/proto/primary/cpp/primary.pb.cc
  thirdparty/can/proto/secondary/cpp/secondary.pb.cc

  thirdparty/lapcounter/src/lapcounter.c
  thirdparty/lapcounter/src/utils/vector/vector.c
  thirdparty/lapcounter/src/utils/vector/point/point.c
)
target_link_libraries(libBase
  hpdf
  canlib
  stdc++fs
  websocketpp::websocketpp
  ${ZeroMQ_LIBRARY}
  ${Protobuf_LIBRARIES}
  Boost::iostreams
  Boost::system
)

if(WITH_CAMERA)
  target_link_libraries(libBase
    ${raspicam_LIBS}
    ${OpenCV_LIBS}
  )
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

## TELEMETRY
add_executable(telemetry scripts/telemetry/main.cpp)
target_link_libraries(telemetry libBase)

add_executable(gps scripts/testGPS/main.cpp)
target_link_libraries(gps libBase)

add_executable(test_can scripts/testCan/main.cpp 
  thirdparty/can/proto/primary/cpp/primary.pb.cc
  thirdparty/can/proto/secondary/cpp/secondary.pb.cc
)
target_link_libraries(test_can ${Protobuf_LIBRARIES} libBase)

## PORT SHARE
add_executable(share scripts/port/share.cpp src/serial.cpp)
add_executable(port_simulator scripts/port/port_simulator.cpp src/serial.cpp )
add_executable(read scripts/port/reader.cpp src/serial.cpp)

## CSV CONVERTER
#add_executable(csv scripts/csv/csv.cpp)
#target_link_libraries(csv
#  libBase
#  libVehicle
#  libJsonModels
#)

## LOG PLAYERS
add_executable(log_player scripts/log_player/log_player.cpp)
target_link_libraries(log_player libBase)
add_executable(gps_log_player scripts/log_player/gps_log_player.cpp)
target_link_libraries(gps_log_player libBase)

# ZMQ TEST
add_executable(test_zmq scripts/testZMQ/test.cpp)
target_link_libraries(test_zmq libBase)

# WS TEST
add_executable(test_ws scripts/testWS/test.cpp)
target_link_libraries(test_ws libBase)