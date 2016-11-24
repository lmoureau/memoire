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
# Copy files                                                          #
################################################################################
add_custom_target(copy_analysis ALL
  cp -ru ${CMAKE_SOURCE_DIR}/analysis ${CMAKE_BINARY_DIR}
  COMMENT "Copying analysis tree")

add_custom_target(copy_lua ALL
  cp -ru ${CMAKE_SOURCE_DIR}/lua ${CMAKE_BINARY_DIR}
  COMMENT "Copying lua tree")

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

# I/O utilitites
add_library(ioutils STATIC histogram_reader.cpp serializer.cpp)
target_link_libraries(ioutils luajit-5.1)

# QCustomplot
add_library(qcustomplot STATIC qcustomplot.cpp)
target_link_libraries(qcustomplot Qt4::QtGui)

# Simple tool to create histograms from an event stream
add_executable(accumulate accumulate.cpp)
target_link_libraries(accumulate ioutils)

# Simple tool to dump events to stdout
add_executable(dump dump.cpp)
target_link_libraries(dump ioutils)

# Simple tool to dump histograms to stdout
add_executable(dumphist dumphist.cpp)
target_link_libraries(dumphist ioutils)

# Advanced tool to create plots with several histograms
add_executable(multiplot multiplot.cpp
  multiplot_window.cpp
  plot_source.cpp)
target_link_libraries(multiplot ioutils qcustomplot)

# Simple tool to read events from a ROOT file
add_executable(readroot readroot.cpp
  castor.cpp
  event.cpp
  parsers.cpp)
target_link_libraries(readroot ${ROOT_LIBS} ioutils)

# Simple tool to show an histogram
add_executable(showhist showhist.cpp)
target_link_libraries(showhist ioutils qcustomplot)

# Simple tool to run code on an event stream
add_executable(process process.cpp)
target_link_libraries(process ioutils)

install(TARGETS memoire dump RUNTIME DESTINATION bin)

################################################################################
# Run analysis                                              #
################################################################################

add_custom_target(run_analysis ALL
  make -j$$(nproc)
  WORKING_DIRECTORY analysis
  DEPENDS accumulate multiplot process readroot
  COMMENT "Running the analysis")
