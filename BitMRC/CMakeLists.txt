project(BitMRC_Server CXX)
cmake_minimum_required(VERSION 2.8)

set(SRCS Addr.cpp base58.cpp BitMRC.cpp endian.cpp main.cpp 
	NodeConnection.cpp packet.cpp pow.cpp utils.cpp NodeBlacklist.cpp)
	
find_library(CRYPTOPP_BASE 
	NAMES libcryptopp.a
	PATHS ${CRYPTOPP_SRC} 
	NO_DEFAULT_PATH)

message("CRYPTOPP_BASE is ${CRYPTOPP_BASE}")
	
add_executable(BitMRC ${SRCS})
target_link_libraries(BitMRC cryptopp pthread)

add_subdirectory(tests)
