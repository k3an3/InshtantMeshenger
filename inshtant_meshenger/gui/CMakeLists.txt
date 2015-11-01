cmake_minimum_required(VERSION 3.0.2)
project(LibMeshenger)

find_package(Qt5Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(chat chat.cpp)
target_link_libraries (chat Qt5::Widgets)

add_executable(chat_test main.cpp)
target_link_libraries (chat_test chat ${LIBMESHENGER})
