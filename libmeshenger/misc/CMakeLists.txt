cmake_minimum_required (VERSION 2.6)
project (TestNode)

SET(CMAKE_CXX_FLAGS "-std=c++11 -g")

include_directories("../include")

add_executable (headless-meshenger test-node.cpp)
add_executable (TestSender test-sender.cpp)

target_link_libraries(TestSender LINK_PUBLIC cryptopp cppnetlib-uri boost_system boost_thread ${LIBMESHENGER})
target_link_libraries(headless-meshenger LINK_PUBLIC cryptopp cppnetlib-uri boost_system boost_thread ${LIBMESHENGER})
