cmake_minimum_required (VERSION 2.6)
project (kikajson_test C)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall")
endif()

add_library(kikajson "kikajson.c")
add_executable(kikajson_test test.c)
target_link_libraries(kikajson_test kikajson)
