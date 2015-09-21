cmake_minimum_required (VERSION 2.6)
project (LibMeshenger)

add_library (libhello hello.cpp)
target_include_directories(libhello PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable (hello main.cpp)
target_link_libraries(hello LINK_PUBLIC libhello)
