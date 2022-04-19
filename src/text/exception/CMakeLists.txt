cmake_minimum_required(VERSION 3.14)

target_sources(${LIBRARY_TEXT}
  PUBLIC
  SyntaxError.hpp

  PRIVATE
  SyntaxError.cpp
)

target_sources(${UNITTEST_TEXT}
  PRIVATE
  SyntaxError.test.cpp
)
