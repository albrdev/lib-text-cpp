cmake_minimum_required(VERSION 3.14)

target_sources(${LIBRARY_TEXT}
  PUBLIC
    SyntaxException.hpp

  PRIVATE
    SyntaxException.cpp
)

target_sources(${UNITTEST_TEXT}
  PRIVATE
    SyntaxException.test.cpp
)
