project(BitMRC_trials CXX)
cmake_minimum_required(VERSION 2.8)

include_directories(./include)

add_executable(dns dns.cpp)
add_executable(thread_test thread_test.cpp)

target_link_libraries(dns pthread)
target_link_libraries(thread_test pthread)