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

################################################################################
# Build configuration                                                          #
################################################################################

add_executable(memoire
  castor.cpp
  event.cpp
  histogram-qt.cpp
  main.cpp
  main_window.cpp
  parsers.cpp
  qcustomplot.cpp
  run.cpp
  run_config.cpp)
# Qt
target_link_libraries(memoire Qt4::QtGui)
# ROOT
target_link_libraries(memoire ${ROOT_LIBS})

install(TARGETS memoire RUNTIME DESTINATION bin)
