cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(arduinoConnect VERSION 1.0)
add_executable(arduinoConnect arduinoConnect.cpp)

find_package(simpleble REQUIRED CONFIG)
target_link_libraries(arduinoConnect simpleble::simpleble)
