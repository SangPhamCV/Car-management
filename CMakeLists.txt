cmake_minimum_required(VERSION 3.10)

project(Dashboard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.h)

add_executable(Dashboard
    ${SOURCES}
)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})

target_link_libraries(Dashboard
    ${CURSES_LIBRARIES}
    pthread
)

target_include_directories(Dashboard
    PRIVATE include
)