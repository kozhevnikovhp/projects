cmake_minimum_required(VERSION 2.8)

project(TrafficCounter)
add_executable(${PROJECT_NAME} "main.cpp")

set(CMAKE_EXE_LINKER_FLAGS " -static")
