cmake_minimum_required(VERSION 3.20)

project(mcc)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(deps/json)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(mcc ${SOURCES})
target_include_directories(mcc PRIVATE "include")
target_link_libraries(mcc PRIVATE json)
