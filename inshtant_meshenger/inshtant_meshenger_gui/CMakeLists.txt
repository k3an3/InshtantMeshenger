cmake_minimum_required(VERSION 3.0.2)
project(inshtant-meshenger)

find_package(Qt5Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(chat main.cpp)
target_link_libraries (chat Qt5::Widgets)

add_executable(chat_test mainwindow.cpp)
target_link_libraries (chat_test LINK_PUBLIC boost_system boost_thread cppnetlib-uri cryptopp chat ${LIBMESHENGER})
