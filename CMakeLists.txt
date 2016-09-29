cmake_minimum_required(VERSION 2.6)
project(memoire)

add_executable(memoire main.cpp)

install(TARGETS memoire RUNTIME DESTINATION bin)
