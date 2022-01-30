cmake_minimum_required(VERSION 3.14)

target_sources(${LIBRARY_TEXT}
  PUBLIC
    ParserBase.hpp
    Parser.hpp
    CommandParser.hpp
    SyntaxException.hpp

  PRIVATE
    ParserBase.cpp
    Parser.cpp
    CommandParser.cpp
    SyntaxException.cpp
)

target_sources(${UNITTEST_TEXT}
  PRIVATE
    Parser.test.cpp
    CommandParser.test.cpp
)

add_subdirectory(format)
add_subdirectory(expression)
