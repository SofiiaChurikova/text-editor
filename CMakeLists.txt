cmake_minimum_required(VERSION 3.28)
project(text-editor)
set(CMAKE_CXX_STANDARD 17)
add_library(caesar SHARED caesar.cpp)
add_executable(text-editor main.cpp)
target_link_libraries(text-editor caesar)#[[//]]
