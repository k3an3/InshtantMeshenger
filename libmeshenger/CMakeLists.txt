cmake_minimum_required (VERSION 2.6)
project (LibMeshenger)

include_directories("include")

include_directories("src/parser")
add_subdirectory("src/parser" "bin")
