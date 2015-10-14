cmake_minimum_required (VERSION 2.6)
project (TestSender)

SET(CMAKE_CXX_FLAGS "-std=c++11")

include_directories("../libmeshenger/include")

add_executable (TestSender main.cpp)

