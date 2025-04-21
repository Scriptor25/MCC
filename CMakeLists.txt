cmake_minimum_required(VERSION 3.31)

project(MCC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(mcc src/main.cpp src/lex.cpp src/parse.cpp)
target_include_directories(mcc PRIVATE include)
