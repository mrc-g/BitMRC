project(BitMRC_Tests CXX)
cmake_minimum_required(VERSION 2.8)

include_directories(./include)
set(TEST_SRC src/EndianTest.cpp)

add_executable(EndianTest ../endian.cpp src/EndianTest.cpp)
add_executable(NodeConnTest ../NodeBlacklist.cpp src/NodeTests.cpp)
add_executable(StorableTest src/StorableTest.cpp ../Addr.cpp  ../Storage/Unique_Key.cpp ../base58.cpp ../utils.cpp)
target_link_libraries(StorableTest cpptest cryptopp)
target_link_libraries(EndianTest cpptest)
target_link_libraries(NodeConnTest cpptest)
