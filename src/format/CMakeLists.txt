cmake_minimum_required(VERSION 3.14)

target_sources(${TARGET_MAIN}
  PUBLIC
    TextFormatter.hpp

  PRIVATE
    TextFormatter.cpp
)

target_sources(${TARGET_TEST}
  PRIVATE
    TextFormatter.test.cpp
)
