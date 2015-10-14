cmake_minimum_required (VERSION 2.6)
project (TestNode)

SET(CMAKE_CXX_FLAGS "-std=c++11")

include_directories("../libmeshenger/include")

add_executable (TestNode main.cpp)

