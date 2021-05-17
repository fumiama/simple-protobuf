cmake_minimum_required(VERSION 3.0.0)
project(simple-protobuf)

include(TestBigEndian)
test_big_endian(isBigEndian)
if (${isBigEndian})
    add_definitions(-DWORDS_BIGENDIAN)
endif()

add_library(spb SHARED protobuf.c)
add_executable(t test.c)
target_link_libraries(t spb)