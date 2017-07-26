project(BitMRC_Server CXX)
cmake_minimum_required(VERSION 2.8)

#include(CMakeForceCompiler)
#CMAKE_FORCE_C_COMPILER(${CMAKE_CXX_COMPILER} C)

set(SRCS Addr.cpp base58.cpp BitMRC.cpp endian.cpp main.cpp
	NodeConnection.cpp packet.cpp pow.cpp utils.cpp NodeBlacklist.cpp
	Storage/Storable.cpp Storage/Unique_Key.cpp
	Storage/Storage_sqlite3.cpp Storage/Storage_mysql.cpp
	Configuration.cpp Storage/StorableConfiguration.cpp
	Storage/QueryResultElement.cpp Storage/QueryResult.cpp Storage/StorableQueryConfig.cpp
	Storage/RowSpec.cpp Storage/ColumnSpec.cpp
	Storage/StorableConfiguration.cpp)

find_library(CRYPTOPP_BASE
	NAMES libcryptopp.a
	PATHS ${CMAKE_SOURCE_DIR}/3rdparty
	NO_DEFAULT_PATH)

link_directories(${CMAKE_SOURCE_DIR}/3rdparty/lib)
add_executable(BitMRC ${SRCS})
target_link_libraries(BitMRC cryptopp pthread sqlite3 mysqlclient)

message("CRYPTOPPBASE: ${CRYPTOPP_BASE}")
if(${ENABLE_TESTS})
	add_definitions("-DENABLE_TESTS -DDEBUG_STORABLES")
	add_subdirectory(tests)
endif()
