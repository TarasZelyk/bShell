cmake_minimum_required(VERSION 3.3)
project(bShell)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/BeerShell )

include_directories("include")

set(SOURCE_FILES main.cpp src/interpreter.cpp include/interpreter.h)
add_executable(bShell ${SOURCE_FILES})

FIND_PACKAGE(Boost COMPONENTS filesystem regex system REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

target_link_libraries(bShell ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

