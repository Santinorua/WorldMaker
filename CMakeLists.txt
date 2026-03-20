cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_COMPILER "g++")

project(WorldMaker VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -----------------------------------------------------------------------------------------------------

file (GLOB PRNG_SRC "${CMAKE_SOURCE_DIR}/Core/PRNG/*.cpp")
add_library(PRNG STATIC ${PRNG_SRC})
target_compile_definitions(PRNG PUBLIC PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")
target_include_directories(PRNG INTERFACE ${CMAKE_SOURCE_DIR}/Core/PRNG)

# -----------------------------------------------------------------------------------------------------
#######################################################################################################
#######################################################################################################
# -----------------------------------------------------------------------------------------------------

add_library(Core INTERFACE)

target_link_libraries(Core INTERFACE PRNG)

# -----------------------------------------------------------------------------------------------------
#######################################################################################################
#######################################################################################################
# -----------------------------------------------------------------------------------------------------

add_executable(WorldMaker WorldMaker.cpp)

target_link_libraries(WorldMaker PUBLIC Core)
