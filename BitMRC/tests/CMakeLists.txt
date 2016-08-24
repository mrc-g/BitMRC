project(BitMRC_Tests CXX)
cmake_minimum_required(VERSION 2.8)

include_directories(./include)
set(TEST_SRC src/EndianTest.cpp)

add_executable(SQLiteTest ../Storage/Storage_sqlite3.cpp src/SQLiteTest.cpp)
add_executable(EndianTest ../endian.cpp src/EndianTest.cpp)
add_executable(NodeConnTest ../NodeBlacklist.cpp src/NodeTests.cpp)
add_executable(StorableTest src/StorableTest.cpp ../Addr.cpp  ../Storage/Unique_Key.cpp ../base58.cpp ../utils.cpp)
add_executable(UtilsTest src/UtilsTest.cpp ../utils.cpp)


target_link_libraries(StorableTest cpptest cryptopp)
target_link_libraries(EndianTest cpptest)
target_link_libraries(NodeConnTest cpptest)
target_link_libraries(UtilsTest cpptest)
# set_target_properties(SQLiteTest PROPERTIES LINKER_LANGUAGE "C")
target_link_libraries(SQLiteTest cpptest sqlite3)