add_library(libmeshenger-net
	net.cpp)

add_executable (nettest main.cpp)

find_package (Boost 1.54.0 REQUIRED COMPONENTS system thread)
if (Boost_FOUND)
	include_directories (SYSTEM ${Boost_INCLUDE_DIR})
endif ()
set (Boost_NO_SYSTEM_PATHS ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME OFF)
set (BOOST_ALL_DYN_LINK OFF)

target_link_libraries(nettest LINK_PUBLIC boost_system boost_thread libmeshenger-net)
target_link_libraries(nettest LINK_PUBLIC boost_system boost_thread libmeshenger-net)
target_link_libraries(nettest LINK_PUBLIC boost_system libmeshenger-parse)
