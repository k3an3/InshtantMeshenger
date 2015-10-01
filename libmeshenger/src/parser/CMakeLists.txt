add_library(libmeshenger-parse
	packet.cpp
	clearmessage.cpp)

add_executable (hello main.cpp)
target_link_libraries(hello LINK_PUBLIC libmeshenger-parse)

