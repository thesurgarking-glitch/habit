cmake_minimum_required(VERSION 3.15)
project(OnwardHabit)

if(NOT DEFINED IPLUG2_ROOT)
  message(FATAL_ERROR "Set -DIPLUG2_ROOT=path/to/iPlug2")
endif()

set(IPLUG2_TARGETS "VST3 AU APP" CACHE STRING "Targets")

add_subdirectory(${IPLUG2_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/iPlug2)

iPlug2_add_plugin(
  NAME OnwardHabit
  PLUG_ID "OHbt"
  COMPANY "YourName"
  MAJOR 1
  MINOR 0
  BUILD 0
  HOSTS VST3 AU APP
  SRCS
    Source/OnwardHabit.cpp
    Source/OnwardHabit.h
  RESOURCES
    Resources/icon.png
)
