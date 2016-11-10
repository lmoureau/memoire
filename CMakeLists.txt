cmake_minimum_required(VERSION 2.8.6 FATAL_ERROR)
project(memoire)

################################################################################
# Dependencies                                                                 #
################################################################################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -pedantic -O2 -g")

# C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

# ROOT
EXECUTE_PROCESS(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")

# Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt4 REQUIRED QtGui)

################################################################################
# Build configuration                                                          #
################################################################################

add_executable(memoire
  event.cpp
  histogram-qt.cpp
  main.cpp
  main_window.cpp
  parser.cpp
  qcustomplot.cpp
  run.cpp
  run_config.cpp)
# Qt
target_link_libraries(memoire Qt4::QtGui)
# ROOT
target_link_libraries(memoire Core RIO Tree)

install(TARGETS memoire RUNTIME DESTINATION bin)
