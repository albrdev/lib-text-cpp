cmake_minimum_required(VERSION 3.14)

target_sources(${LIBRARY_TEXT}
  PUBLIC
  Common.hpp

  PRIVATE
  Common.cpp
)

target_sources(${UNITTEST_TEXT}
  PRIVATE
  Common.test.cpp
)

add_subdirectory(parse)
add_subdirectory(format)
add_subdirectory(expression)
add_subdirectory(exception)
