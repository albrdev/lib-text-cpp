cmake_minimum_required(VERSION 3.14)

#set(CMAKE_C_STANDARD 98)
#set(CMAKE_C_STANDARD_REQUIRED True)
#set(CMAKE_C_COMPILER "gcc")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_CXX_COMPILER "g++")

set(LIBRARY_TEXT text)
set(UNITTEST_TEXT text-test)

if(TARGET ${LIBRARY_TEXT})
    return()
endif()

project(lib-text VERSION 1.0.0)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -Wpedantic -fconcepts -Wno-psabi")
elseif(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

add_library(${LIBRARY_TEXT} STATIC)
set_target_properties(${LIBRARY_TEXT} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${LIBRARY_TEXT} PUBLIC src ext/lib-common-cpp/src)

add_library(${UNITTEST_TEXT} STATIC)
target_link_libraries(${UNITTEST_TEXT} ${LIBRARY_TEXT} gtest_main gmock_main)

# Traverse directories
add_subdirectory(src)
add_subdirectory(ext/lib-common-cpp)

set(LIBRARY_COMMON common)
target_link_libraries(${LIBRARY_TEXT} ${LIBRARY_COMMON})

set(PROJECT_TEST unit_testsuite-text)
set(EXECUTABLE_TEST unit_testsuite-text)

project(${PROJECT_TEST})

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

get_target_property(SOURCES ${UNITTEST_TEXT} SOURCES)
add_executable(${EXECUTABLE_TEST} main-test.cpp ${SOURCES})
target_link_libraries(${EXECUTABLE_TEST} ${UNITTEST_TEXT})
enable_testing()
add_test(NAME ${PROJECT_TEST} COMMAND ${EXECUTABLE_TEST})
