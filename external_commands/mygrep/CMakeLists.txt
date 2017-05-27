cmake_minimum_required(VERSION 3.6)
project(mygrep)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp)

add_executable(mygrep ${SOURCE_FILES})

FIND_PACKAGE(Boost COMPONENTS system program_options regex filesystem signals thread REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
target_link_libraries(mygrep ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(mygrep PUBLIC SYSTEM ${Boost_INCLUDE_DIRS})