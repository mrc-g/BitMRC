project(BitMRC CXX)
cmake_minimum_required(VERSION 2.8)
include(GNUInstallDirs)
set(CMAKE_VERBOSE_MAKEFILE true)
set(CRYPTOPP_INC ${CMAKE_SOURCE_DIR}/cryptopp)
set(CRYPTOPP_SRC ${CMAKE_SOURCE_DIR}/cryptopp)
message("System Type ${CMAKE_SYSTEM}")
 
include_directories(/usr/local/include/cryptopp ${CMAKE_SOURCE_DIR}/BitMRC/include
	/usr/include/c++/4.9.3 /usr/include/c++)

execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE CXX_VER)
message("CXXVER IS : ${CXX_VER}")
if("${CXX_VER}" MATCHES " 4.9." OR  "${CXX_VER}" MATCHES " 5.")
	message("c++ version is okay")
else()
	message(FATAL_ERROR "\n***** ERROR: Need g++ >= 4.9 for this... (shared_timed_mutex)")
endif()
if(${CMAKE_SYSTEM} MATCHES "Linux")
	# TODO: check 'permissive' flag.
	add_definitions(-DLINUX -std=c++1y -fpermissive)
else()
	# TODO: add windows definition
endif()
include(ExternalProject)
ExternalProject_Add(libcryptopp 
		# GIT_REPOSITORY https://github.com/weidai11/cryptopp.git
		# FOR TESTING: use a local bare repo
		GIT_REPOSITORY ${CMAKE_SOURCE_DIR}/../libcryptopp.git
		PREFIX ${CRYPTOPP_SRC}
		BUILD_IN_SOURCE 1
		CONFIGURE_COMMAND cmake . -DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_SOURCE_DIR}/libs
		)
add_subdirectory(BitMRC)