add_library(libmeshenger-tracker
	tracker.cpp)

add_executable (tracker_test main.cpp)

find_package (Boost 1.58.0 REQUIRED COMPONENTS system thread)
if (Boost_FOUND)
	include_directories (SYSTEM ${Boost_INCLUDE_DIR})
endif ()
set (Boost_NO_SYSTEM_PATHS ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME OFF)
set (BOOST_ALL_DYN_LINK OFF)

target_link_libraries(tracker_test LINK_PUBLIC cryptopp boost_system boost_thread ${LIBMESHENGER})
