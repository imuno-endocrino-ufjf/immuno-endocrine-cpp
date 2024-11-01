cmake_minimum_required(VERSION 3.28)
project(immuno-endocrine-cpp VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(fmt CONFIG REQUIRED)

add_executable(immuno-endocrine-cpp src/main.cpp)
target_link_libraries(immuno-endocrine-cpp PRIVATE fmt::fmt)
