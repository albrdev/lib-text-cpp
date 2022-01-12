cmake_minimum_required(VERSION 3.14)

target_sources(${TARGET_MAIN}
  PUBLIC
    ParserBase.hpp
    Parser.hpp

  PRIVATE
    ParserBase.cpp
    Parser.cpp
)

target_sources(${TARGET_TEST}
  PRIVATE
    Parser.test.cpp
)

add_subdirectory(format)
add_subdirectory(expression)
