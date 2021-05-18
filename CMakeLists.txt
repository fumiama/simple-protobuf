cmake_minimum_required(VERSION 2.6)
project(simple-protobuf C)

include(TestBigEndian)
test_big_endian(isBigEndian)
if (${isBigEndian})
    add_definitions(-DWORDS_BIGENDIAN)
endif()

add_library(spb SHARED protobuf.c)
add_library(spb_static STATIC protobuf.c)
set_target_properties(spb_static PROPERTIES OUTPUT_NAME spb)
add_executable(t test.c)
target_link_libraries(t spb)

INSTALL(TARGETS spb LIBRARY DESTINATION lib)
INSTALL(TARGETS spb_static ARCHIVE DESTINATION lib)
INSTALL(FILES simple_protobuf.h DESTINATION include)