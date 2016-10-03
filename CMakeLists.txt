cmake_minimum_required(VERSION 2.8.6)
project(memoire)

################################################################################
# Dependencies                                                                 #
################################################################################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -pedantic -O2 -g")

# C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt4 REQUIRED QtGui)

################################################################################
# Build configuration                                                          #
################################################################################

add_executable(memoire
  event.cpp
  main.cpp
  main_window.cpp
  parser.cpp
  qcustomplot.cpp
  run.cpp
  run_config.cpp)
target_link_libraries(memoire Qt4::QtGui)

install(TARGETS memoire RUNTIME DESTINATION bin)
