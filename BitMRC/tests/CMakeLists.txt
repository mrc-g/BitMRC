project(BitMRC_Tests CXX)
cmake_minimum_required(VERSION 2.8)

include_directories(./include)
set(TEST_SRC src/EndianTest.cpp)


add_library(libbitmrc  ../NodeBlacklist.cpp ../utils.cpp ../base58.cpp ../endian.cpp  ../Addr.cpp ../Storage/Unique_Key.cpp ../packet.cpp
	 ../Configuration.cpp  ../Storage/Storable.cpp ../Storage/Storage_sqlite3.cpp ../Storage/Storage_mysql.cpp  ../Storage/StorableQueryConfig.cpp
	../Storage/QueryResult.cpp ../Storage/QueryResultElement.cpp ../Storage/RowSpec.cpp ../Storage/ColumnSpec.cpp
	../Storage/StorableConfiguration.cpp)


add_executable(SQLiteTest src/SQLiteTest.cpp)
add_executable(MySQLTest src/MySQLTest.cpp)
add_executable(EndianTest  src/EndianTest.cpp)
add_executable(NodeConnTest src/NodeTests.cpp)
add_executable(StorableTest src/StorableTest.cpp)
add_executable(StorableAddressTest src/StorableAddressTest.cpp)
add_executable(UtilsTest src/UtilsTest.cpp)
add_executable(StorableConfigurationTest src/StorableConfigurationTest.cpp)
add_executable(ColumnSpecTest src/Storage/ColumnSpecTest.cpp)
add_executable(RowColumnTest src/Storage/RowColumnTest.cpp  src/Storage/RowColumnTest.cpp)
add_executable(StorablePacketTest src/StorablePacketTest.cpp )

target_link_libraries(StorableTest cpptest cryptopp sqlite3 libbitmrc cryptopp sqlite3)
target_link_libraries(StorableAddressTest cpptest cryptopp mysqlclient libbitmrc cryptopp)
target_link_libraries(StorableConfigurationTest cpptest cryptopp sqlite3 libbitmrc cryptopp sqlite3 mysqlclient)
target_link_libraries(EndianTest cpptest libbitmrc)
target_link_libraries(NodeConnTest cpptest libbitmrc)
target_link_libraries(UtilsTest cpptest libbitmrc)
target_link_libraries(ColumnSpecTest cpptest cryptopp libbitmrc sqlite3)
target_link_libraries(RowColumnTest cpptest cryptopp libbitmrc sqlite3)
target_link_libraries(StorablePacketTest cpptest cryptopp libbitmrc mysqlclient cryptopp libbitmrc)

# set_target_properties(SQLiteTest PROPERTIES LINKER_LANGUAGE "C")
target_link_libraries(SQLiteTest cpptest libbitmrc sqlite3 cryptopp)
target_link_libraries(MySQLTest cpptest libbitmrc mysqlclient cryptopp)