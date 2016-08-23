project(BitMRC_Server CXX)
cmake_minimum_required(VERSION 2.8)

#include(CMakeForceCompiler)
#CMAKE_FORCE_C_COMPILER(${CMAKE_CXX_COMPILER} C)

set(SRCS Addr.cpp base58.cpp BitMRC.cpp endian.cpp main.cpp
	NodeConnection.cpp packet.cpp pow.cpp utils.cpp NodeBlacklist.cpp
	Storage/Unique_Key.cpp Storage/Storage_sqlite3.cpp)

find_library(CRYPTOPP_BASE
	NAMES libcryptopp.a
	PATHS ${CMAKE_SOURCE_DIR}/3rdparty
	NO_DEFAULT_PATH)

link_directories(${CMAKE_SOURCE_DIR}/3rdparty/lib)
add_executable(BitMRC ${SRCS})
target_link_libraries(BitMRC cryptopp pthread sqlite3)

message("CRYPTOPPBASE: ${CRYPTOPP_BASE}")
if(${ENABLE_TESTS})
	add_definitions("-DENABLE_TESTS")
	add_subdirectory(tests)
endif()
