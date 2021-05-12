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

find_package(nlohmann_json)
if(${nlohmann_json_FOUND})
  add_definitions(-DJSON=1)
else()
  message("JSON not found!")
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/include
  )

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(
  ${PROJECT_NAME} SHARED
  src/can.cpp
  src/utils.cpp
  src/browse.cpp
  src/devices.cpp
  src/vehicle.cpp
  )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(telemetry scripts/telemetry.cpp)
target_link_libraries(telemetry ${PROJECT_NAME} ${Boost_LIBRARIES} jsoncpp)

add_executable(csv scripts/csv.cpp)
target_link_libraries(csv ${PROJECT_NAME} ${Boost_LIBRARIES} jsoncpp)
