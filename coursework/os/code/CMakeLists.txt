project(os_lab)
cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output)

add_subdirectory(conf)
add_subdirectory(src/emsh)
add_subdirectory(src/test)
add_subdirectory(src/mkdsk)
