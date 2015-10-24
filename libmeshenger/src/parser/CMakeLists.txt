add_library(libmeshenger-parse
	packet.cpp
	clearmessage.cpp)

find_library (Cryptopp crypto++ /usr/lib64)
if (Cryptopp_FOUND)
	include_directories (SYSTEM ${Cryptopp_INCLUDE_DIR})
endif ()

add_executable (hello main.cpp)
target_link_libraries(hello LINK_PUBLIC libmeshenger-parse cryptopp)

