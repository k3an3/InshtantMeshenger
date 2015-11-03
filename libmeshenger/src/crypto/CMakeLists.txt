find_library (Cryptopp crypto++ /usr/lib64)
if (Cryptopp_FOUND)
	include_directories (SYSTEM ${Cryptopp_INCLUDE_DIR})
endif ()

add_library(libmeshenger-crypto
	crypto.cpp)

add_executable (test-crypto main.cpp)
target_link_libraries(test-crypto LINK_PUBLIC cryptopp ${LIBMESHENGER})

add_executable (keygen keygen.cpp)
target_link_libraries(keygen LINK_PUBLIC cryptopp ${LIBMESHENGER})
