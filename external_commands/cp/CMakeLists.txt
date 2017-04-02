cmake_minimum_required(VERSION 3.6)
project(cp)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp)

add_executable(cp ${SOURCE_FILES})

FIND_PACKAGE(Boost COMPONENTS system program_options filesystem signals thread REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
target_link_libraries(cp ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(cp PUBLIC SYSTEM ${Boost_INCLUDE_DIRS})