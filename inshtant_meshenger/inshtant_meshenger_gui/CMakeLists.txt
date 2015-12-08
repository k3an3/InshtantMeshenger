cmake_minimum_required(VERSION 3.0.2)
project(inshtant-meshenger)

find_package(Qt5Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${Qt5Widgets_INCLUDE_DIRS})

add_executable(inshtant_meshenger mainwindow.cpp addpeer.cpp pgp.cpp main.cpp)
target_link_libraries (inshtant_meshenger LINK_PUBLIC boost_system boost_thread Qt5::Widgets cppnetlib-uri cryptopp ${LIBMESHENGER})
