cmake_minimum_required (VERSION 2.6)
project (TestNode)

SET(CMAKE_CXX_FLAGS "-std=c++11")

include_directories("../include")

add_executable (TestNode test-node.cpp)
add_executable (TestSender test-sender.cpp)

target_link_libraries(TestSender LINK_PUBLIC boost_system boost_thread libmeshenger-net)
target_link_libraries(TestSender LINK_PUBLIC libmeshenger-state libmeshenger-parse)
target_link_libraries(TestSender LINK_PUBLIC cryptopp)
target_link_libraries(TestNode LINK_PUBLIC boost_system libmeshenger-net)
target_link_libraries(TestNode LINK_PUBLIC boost_thread)
target_link_libraries(TestNode LINK_PUBLIC libmeshenger-state libmeshenger-parse)
target_link_libraries(TestNode LINK_PUBLIC cryptopp)
