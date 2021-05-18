cmake_minimum_required(VERSION 2.6)
project(simple-protobuf C)

include(TestBigEndian)
test_big_endian(isBigEndian)
if (${isBigEndian})
    add_definitions(-DWORDS_BIGENDIAN)
endif()

add_library(spb SHARED protobuf.c)
add_executable(t test.c)
target_link_libraries(t spb)

INSTALL(TARGETS spb LIBRARY DESTINATION lib)
INSTALL(TARGETS spb PUBLIC_HEADER DESTINATION include)
