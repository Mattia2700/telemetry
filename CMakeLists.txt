cmake_minimum_required(VERSION 3.10)

project(Telemetry)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost  1.54.0
   REQUIRED
   COMPONENTS serialization filesystem
)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/include
  )

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(
  ${PROJECT_NAME} SHARED
  src/can.cpp
  )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(telemetry scripts/telemetry.cpp)
target_link_libraries(telemetry ${PROJECT_NAME} ${Boost_LIBRARIES})
