cmake_minimum_required(VERSION 3.14)

target_sources(${LIBRARY_TEXT}
  PUBLIC
    TextFormatter.hpp

  PRIVATE
    TextFormatter.cpp
)

target_sources(${UNITTEST_TEXT}
  PRIVATE
    TextFormatter.test.cpp
)
