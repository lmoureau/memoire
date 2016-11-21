cmake_minimum_required(VERSION 2.8.6 FATAL_ERROR)
project(memoire)

################################################################################
# Dependencies                                                                 #
################################################################################

# Lua
find_package(Lua51 REQUIRED)
include_directories(${LUA_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -pedantic -O2 -g")

# ROOT
EXECUTE_PROCESS(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
EXECUTE_PROCESS(COMMAND root-config --libdir OUTPUT_VARIABLE ROOT_LIB_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")

find_library(ROOT_LIBCORE Core ${ROOT_LIB_DIR})
find_library(ROOT_LIBRIO  RIO ${ROOT_LIB_DIR})
find_library(ROOT_LIBTREE Tree ${ROOT_LIB_DIR})
set(ROOT_LIBS "${ROOT_LIBCORE}" "${ROOT_LIBRIO}" "${ROOT_LIBTREE}")

# Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt4 REQUIRED QtGui)

# C++14 for sol
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

################################################################################
# Build configuration                                                          #
################################################################################

add_executable(memoire
  cached_event_source.cpp
  castor.cpp
  cut.cpp
  event.cpp
  histogram-qt.cpp
  main.cpp
  main_window.cpp
  parsers.cpp
  qcustomplot.cpp
  run.cpp
  run_config.cpp
  serializer.cpp)
# Luajit
target_link_libraries(memoire luajit-5.1)
# Qt
target_link_libraries(memoire Qt4::QtGui)
# ROOT
target_link_libraries(memoire ${ROOT_LIBS})

# Simple tool to create histograms from an event stream
add_executable(accumulate accumulate.cpp serializer.cpp)
target_link_libraries(accumulate luajit-5.1)

# Simple tool to dump events to stdout
add_executable(dump dump.cpp serializer.cpp)
target_link_libraries(dump luajit-5.1)

# Simple tool to dump histograms to stdout
add_executable(dumphist dumphist.cpp serializer.cpp)
target_link_libraries(dumphist luajit-5.1)

# Simple tool to read events from a ROOT file
add_executable(readroot readroot.cpp
  castor.cpp
  event.cpp
  parsers.cpp
  serializer.cpp)
target_link_libraries(readroot ${ROOT_LIBS} luajit-5.1)

# Simple tool to run code on an event stream
add_executable(process process.cpp serializer.cpp)
target_link_libraries(process luajit-5.1)

install(TARGETS memoire dump RUNTIME DESTINATION bin)
