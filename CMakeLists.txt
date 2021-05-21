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

find_package(CURL REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${CURL_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/include
  )

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(
  ${PROJECT_NAME} SHARED
  src/can.cpp
  src/utils.cpp
  src/browse.cpp
  src/serial.cpp
  src/devices.cpp
  src/vehicle.cpp
  )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(telemetry scripts/telemetry/telemetry.cpp)
target_link_libraries(telemetry ${PROJECT_NAME} ${Boost_LIBRARIES})

add_executable(csv scripts/csv/csv.cpp)
target_link_libraries(csv ${PROJECT_NAME} ${Boost_LIBRARIES})

add_executable(checker scripts/checker/checker.cpp)
target_link_libraries(checker ${PROJECT_NAME} ${Boost_LIBRARIES})

add_executable(dashboard scripts/dashboard/dashboard.cpp)
target_link_libraries(dashboard ${PROJECT_NAME} ${Boost_LIBRARIES} ${CURL_LIBRARIES})
