cmake_minimum_required(VERSION 3.28)
project(parallel__homework_1)

set(CMAKE_CXX_STANDARD 17)

add_executable(parallel__homework_1 main.cpp
        quicksort.h)

include(FetchContent)
FetchContent_Declare(parlaylib
        GIT_REPOSITORY  https://github.com/cmuparlay/parlaylib.git
        GIT_TAG         master
)
FetchContent_GetProperties(parlaylib)
if(NOT parlaylib_POPULATED)
    FetchContent_Populate(parlaylib)
    add_subdirectory(${parlaylib_SOURCE_DIR} EXCLUDE_FROM_ALL)
endif()


target_link_libraries(parallel__homework_1 PUBLIC parlay)