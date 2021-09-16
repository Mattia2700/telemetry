cmake_minimum_required(VERSION 3.10)

project(Telemetry)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")

###########
## BOOST ##
###########
find_package(Boost  1.54.0
   REQUIRED
   COMPONENTS serialization filesystem
)

find_package(CURL REQUIRED)

find_package(PythonLibs 2.7 REQUIRED)

find_package(Protobuf REQUIRED)
if(Protobuf_FOUND)
    message(STATUS "Using Protocol Buffers ${Protobuf_VERSION}")
endif()

find_package(websocketpp)
if(websocketpp_FOUND)
  message(STATUS "Websocketpp FOUND")
endif()


include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/inc
  ${PROJECT_SOURCE_DIR}/thirdparty
  ${PROJECT_SOURCE_DIR}/Protobuffer/cpp

  ${CURL_INCLUDE_DIR}
  )


if(PythonLibs_FOUND)
  include_directories(
    ${PYTHON_INCLUDE_DIRS}
  )
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(
  ${PROJECT_NAME} SHARED
  src/can.cpp
  src/utils.cpp
  src/browse.cpp
  src/serial.cpp
  src/devices.cpp
  src/vehicle.cpp
  Protobuffer/cpp/devices.pb.cc
  )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(logger scripts/logger/logger.cpp)
target_link_libraries(logger ${PROJECT_NAME} ${Protobuf_LIBRARIES} ${Boost_LIBRARIES})

add_executable(telemetry scripts/telemetry/telemetry.cpp)
target_link_libraries(telemetry ${PROJECT_NAME} ${Protobuf_LIBRARIES} ${Boost_LIBRARIES})

add_executable(csv scripts/csv/csv.cpp)
target_link_libraries(csv ${PROJECT_NAME} ${Protobuf_LIBRARIES} ${Boost_LIBRARIES})

add_executable(checker scripts/checker/checker.cpp)
target_link_libraries(checker ${PROJECT_NAME} ${Protobuf_LIBRARIES} ${Boost_LIBRARIES})

add_executable(dashboard scripts/dashboard/dashboard.cpp)
target_link_libraries(dashboard ${PROJECT_NAME} ${Protobuf_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES})

add_executable(share scripts/port/share.cpp)
target_link_libraries(share ${PROJECT_NAME} ${Protobuf_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES})

add_executable(read scripts/port/reader.cpp)
target_link_libraries(read ${PROJECT_NAME} ${Protobuf_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES})


if(websocketpp_FOUND)
  add_executable(dashboard_can scripts/dashboard_can/dashboard_can.cpp)
  target_link_libraries(dashboard_can ${PROJECT_NAME} ${Protobuf_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES} websocketpp::websocketpp)
  add_executable(websocket scripts/websocket/websocket.cpp)
  target_link_libraries(websocket ${PROJECT_NAME} ${Protobuf_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES} websocketpp::websocketpp)
endif()

if(PythonLibs_FOUND)
  add_executable(graph scripts/graph/graph.cpp)
  target_link_libraries(graph ${PROJECT_NAME} ${Protobuf_LIBRARIES} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
endif()
