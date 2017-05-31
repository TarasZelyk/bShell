cmake_minimum_required(VERSION 3.3)
project(BeerShell)

set(CMAKE_CXX_STANDARD 11)
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/BeerShell/bin )

add_subdirectory(bShell)
add_subdirectory(external_commands/cp)
add_subdirectory(external_commands/ls)
add_subdirectory(external_commands/mkdir)
add_subdirectory(external_commands/mv)
add_subdirectory(external_commands/rm)
add_subdirectory(external_commands/mygrep)

