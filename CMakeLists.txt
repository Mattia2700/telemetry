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

find_package(libOpenGLCustom REQUIRED PATHS /usr/local/lib/cmake/libOpenGLCustom)
if(libOpenGLCustom_FOUND)
  message("----- libOpenGLCustom_FOUND")
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
  src/proto_deserializer.cpp
  Protobuffer/cpp/devices.pb.cc

  # JSON
  json_models/telemetry_config/json_loader.cpp
  json_models/session_config/json_loader.cpp
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


find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

add_executable(share scripts/port/share.cpp src/serial.cpp)
add_executable(port_simulator scripts/port/port_simulator.cpp src/serial.cpp )
add_executable(read scripts/port/reader.cpp src/serial.cpp)


if(websocketpp_FOUND AND Protobuf_FOUND AND Boost_FOUND)
  add_executable(dashboard_simulator scripts/dashboard/dashboard.cpp)
  target_link_libraries(dashboard_simulator ${Protobuf_LIBRARIES} ${Boost_LIBRARIES} websocketpp::websocketpp ${PROJECT_NAME})

  add_executable(dashboard_can scripts/dashboard_can/dashboard_can.cpp)
  target_link_libraries(dashboard_can ${Protobuf_LIBRARIES} ${Boost_LIBRARIES} websocketpp::websocketpp ${PROJECT_NAME})

  add_library(libProtoDevices
              SHARED
              Protobuffer/cpp/devices.pb.cc
              src/proto_deserializer.cpp
              )
endif()
