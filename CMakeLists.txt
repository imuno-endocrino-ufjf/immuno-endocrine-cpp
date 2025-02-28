cmake_minimum_required(VERSION 3.28)
cmake_policy(SET CMP0167 OLD) # change this in the future when cmake 3.30 becomes more ubiquitous
project(immuno-endocrine-cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

add_executable(immuno-endocrine-cpp
    src/main.cpp
    src/utilities.cpp
    src/cortisol_cytokines_model.cpp
    src/cortisol_cytokines_values.cpp
    src/cortisol_cytokines_simulation.cpp
)
target_include_directories(immuno-endocrine-cpp PUBLIC "${PROJECT_SOURCE_DIR}/include")

if(WIN32)
    set(CMAKE_CXX_COMPILER "cl")
endif()

if(ENABLE_DEBUG)
    set(CMAKE_BUILD_TYPE "Debug")
else()
    set(CMAKE_BUILD_TYPE "Release")
endif()

find_package(Boost REQUIRED COMPONENTS numeric_odeint)
target_link_libraries(immuno-endocrine-cpp PRIVATE Boost::numeric_odeint)

find_package(Matplot++ REQUIRED CONFIG)
target_link_libraries(immuno-endocrine-cpp PRIVATE Matplot++::matplot Matplot++::cimg)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(immuno-endocrine-cpp PRIVATE fmt::fmt)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(immuno-endocrine-cpp PRIVATE nlohmann_json::nlohmann_json)
