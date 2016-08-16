project(BitMRC_Server CXX)
cmake_minimum_required(VERSION 2.8)

#include(CMakeForceCompiler)
#CMAKE_FORCE_C_COMPILER(${CMAKE_CXX_COMPILER} C)

set(SRCS Addr.cpp base58.cpp BitMRC.cpp endian.cpp main.cpp
	NodeConnection.cpp packet.cpp pow.cpp utils.cpp NodeBlacklist.cpp
	Storage/Unique_Key.cpp Storage/Storage_sqlite3.cpp)
	
find_library(CRYPTOPP_BASE 
	NAMES libcryptopp.a
	PATHS ${CRYPTOPP_SRC} 
	NO_DEFAULT_PATH)

# message("CRYPTOPP_BASE is ${CRYPTOPP_BASE} CXX ${CMAKE_CXX_COMPILER} C  ${CMAKE_C_COMPILER}")
	
add_executable(BitMRC ${SRCS})
target_link_libraries(BitMRC cryptopp pthread sqlite3)

if(${ENABLE_TESTS})
	add_definitions("-DENABLE_TESTS")
	add_subdirectory(tests)
endif()
