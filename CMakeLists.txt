project(BitMRC CXX)
cmake_minimum_required(VERSION 2.8.11) # 2.8.11 learned target_include_directories
include(GNUInstallDirs)
include(Options.cmake)
set(CRYPTOPP_INC ${CMAKE_SOURCE_DIR}/libcryptopp)
set(CRYPTOPP_SRC ${CMAKE_SOURCE_DIR}/libcryptopp)
set(CMAKE_VERBOSE_MAKEFILE false)

include_directories(${CMAKE_SOURCE_DIR}/3rdparty/include/cryptopp
 	${CMAKE_SOURCE_DIR}/BitMRC/include ${CMAKE_SOURCE_DIR}/BitMRC/Storage/include
 	/usr/include/c++/4.9.3 /usr/include/c++)

if(DBG_DB)
	add_definitions("-DLOG_DB_TO_STDOUT")
endif()
if(DBG_STORAGE_CLASSES)
add_definitions("-DDEBUG_UQK  -DDEBUG_STORAGE -DDEBUG_CSPEC -DDEBUG_RSPEC -DDEBUG_STORABLES")
endif()

execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE CXX_VER)

if("${CXX_VER}" MATCHES " 4.9." OR  "${CXX_VER}" MATCHES " 5.")
	# message("c++ version is okay")
else()
	message(FATAL_ERROR "\n***** ERROR: Need g++ >= 4.9 for this... (shared_timed_mutex)")
endif()
if(${CMAKE_SYSTEM} MATCHES "Linux")
	# TODO: check 'permissive' flag.
	add_definitions(-DLINUX -std=c++1y -fpermissive -o0 -ggdb)
	find_library(LIBCPPTEST_PATH NAMES cpptest libcpptest.a)
	if(${LIBCPPTEST_PATH} MATCHES NOTFOUND)
		message(FATAL_ERROR "\n***** ERROR: Need libcpptest-dev\n")
	endif()
else()
	# TODO: add windows definition
	# TODO: add find_library libcpptest for windows
endif()

if(BUILD_LIBCRYPTOPP_LOCAL)
	set(LCRPP_LOCAL_PATH ${LIBCRYPTOPP_LOCAL_PATH})
else()
	set(LCRPP_LOCAL_PATH "https://github.com/weidai11/cryptopp.git")
endif()



include(ExternalProject)
ExternalProject_Add(libcryptopp
		GIT_REPOSITORY ${LCRPP_LOCAL_PATH}
		PREFIX ${CRYPTOPP_SRC}
		BUILD_IN_SOURCE 1
		CONFIGURE_COMMAND cmake . -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/3rdparty/
		INSTALL_COMMAND make install PREFIX=${CMAKE_SOURCE_DIR}/3rdparty/
		STAMP_DIR ${CRYPTOPP_SRC}
		)
add_subdirectory(BitMRC)
#
# todo: move this to ccmake config
# for now enable by cmake -DENABLE_TRIALS=1
if(${ENABLE_TRIALS})
  add_subdirectory(trial)
endif()

