cmake_minimum_required(VERSION 3.3)
project(bShell)

set(CMAKE_CXX_STANDARD 11)

include_directories("include")

FIND_PACKAGE(Boost COMPONENTS filesystem regex system REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})


SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")

set(SOURCE_FILES main.cpp src/interpreter.cpp include/interpreter.h)
add_executable(bShell ${SOURCE_FILES})
target_link_libraries(bShell ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
