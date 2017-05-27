cmake_minimum_required(VERSION 3.6)
project(bShell)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp)

add_executable(bShell ${SOURCE_FILES})

FIND_PACKAGE(Boost COMPONENTS system program_options filesystem signals thread REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
target_link_libraries(bShell ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(bShell PUBLIC SYSTEM ${Boost_INCLUDE_DIRS})