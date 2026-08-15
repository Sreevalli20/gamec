cmake_minimum_required(VERSION 3.20)
project(ChronoRaid VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE CHRONORAID_SOURCES 
    "src/chronoraid/**/*.cpp"
    "src/chronoraid/**/*.cc"
)

file(GLOB_RECURSE CHRONORAID_HEADERS 
    "include/chronoraid/**/*.h"
    "include/chronoraid/**/*.hpp"
)

add_executable(chronoraid
    src/chronoraid/main.cpp
    ${CHRONORAID_SOURCES}
)

target_include_directories(chronoraid PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

enable_testing()

file(GLOB_RECURSE TEST_SOURCES 
    "tests/**/*.cpp"
    "tests/**/*.cc"
)

foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source} ${CHRONORAID_SOURCES})
    target_include_directories(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
