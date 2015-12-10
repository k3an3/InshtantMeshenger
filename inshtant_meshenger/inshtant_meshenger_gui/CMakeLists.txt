cmake_minimum_required(VERSION 3.0.2)
project(inshtant-meshenger)

find_package(Qt5Widgets)
find_package(Qt5Multimedia)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Multimedia_INCLUDE_DIRS})

add_executable(inshtant_meshenger mainwindow.cpp addpeer.cpp pgp.cpp main.cpp addbuddy.cpp)
target_link_libraries (inshtant_meshenger LINK_PUBLIC boost_system boost_thread Qt5::Widgets Qt5::Multimedia cppnetlib-uri cryptopp ${LIBMESHENGER})
