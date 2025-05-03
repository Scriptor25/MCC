cmake_minimum_required(VERSION 3.31)

project(MCC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

add_subdirectory(deps/json)

file(GLOB_RECURSE sources src/*.cpp)
add_executable(mcc ${sources})
target_include_directories(mcc PRIVATE include)
target_link_libraries(mcc PRIVATE nlohmann_json::nlohmann_json)
