cmake_minimum_required(VERSION 3.0.0)
project(simple-protobuf)

include(TestBigEndian)
test_big_endian(isBigEndian)
if (${isBigEndian})
    add_library(spb SHARED protobuf_be.c)
else()
    add_library(spb SHARED protobuf_le.c)
endif()

add_executable(test test.c)
target_link_libraries(test spb)