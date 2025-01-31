cmake_minimum_required(VERSION 3.15)

project(magics128)

set(CMAKE_CXX_STANDARD 20)

option(FAST_PEXT "whether pext and pdep are usably fast on this architecture" ON)

add_executable(magics128 src/main.cpp src/types.h src/core.h src/util/bits.h src/bitboard.h src/util/multi_array.h
	src/pext/util.h src/pext/data.h src/pext/pext.h src/pext/pext.cpp src/util/rng.h src/util/blocking_queue.h
)

target_compile_options(magics128 PUBLIC -march=native)

target_compile_definitions(magics128 PUBLIC ST_NATIVE)

if(FAST_PEXT)
	target_compile_definitions(magics128 PUBLIC ST_FAST_PEXT)
endif()
