cmake_minimum_required(VERSION 3.7)
project(SpicECS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/)

if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json")
  EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
  )
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")

include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
    set(CTEST_BUILD_FLAGS -j${N})
    set(ctest_test_args ${ctest_test_args} PARALLEL_LEVEL ${N})
endif()

file(GLOB_RECURSE ECS_DIR
    "ecs/*.h"
    "ecs/reflection/*.h"
    "ecs/componentfactory/*.h"
    )

file(GLOB_RECURSE SERIALIZATION_DIR
    "serialization/*.h"
    "serialization/*.cpp"
    )

file(GLOB_RECURSE RAPIDJSON_DIR
    "3rdparty/rapidjson/*.h"
    "3rdparty/rapidjson/err/*.h"
    )

file(GLOB_RECURSE UTIL_DIR
    "util/*.h"
    )

file(GLOB_RECURSE COMPS_DIR
    "example_components/*.h"
    "example_components/*.cpp"
    )

add_executable(SpicECS ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${ECS_DIR} ${SERIALIZATION_DIR} ${RAPIDJSON_DIR} ${UTIL_DIR} ${COMPS_DIR})

find_package(Boost REQUIRED)
include_directories(SpicECS ${Boost_INCLUDE_DIRS})
target_link_libraries(SpicECS ${Boost_LIBRARIES})

target_link_libraries(SpicECS PUBLIC stdc++fs)
