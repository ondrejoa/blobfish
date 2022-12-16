cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
add_subdirectory(libblobfish)

project(blobfish)

set(BF_DEBUG_OPTIONS -ggdb -O0 -Wall -Wextra -pedantic -march=native -fsanitize=address -fno-omit-frame-pointer -pthread)
set(BF_RELEASE_OPTIONS -O3 -Wall -Wextra -pedantic -march=native -pthread)
set(BF_DEBUG_LIBRARIES -lasan -lblobfish -lmagic)
set(BF_RELEASE_LIBRARIES -lblobfish -lmagic)

set(BF_DEBUG_LIBDIR "${CMAKE_SOURCE_DIR}/cmake-build-debug/libblobfish")
set(BF_RELEASE_LIBDIR "${CMAKE_SOURCE_DIR}/cmake-build-release/libblobfish")

add_executable(blobfish blobfish/main.cpp)
add_dependencies(blobfish libblobfish)
target_include_directories(blobfish PRIVATE libblobfish)
target_link_directories(blobfish PRIVATE "$<$<CONFIG:DEBUG>:${BF_DEBUG_LIBDIR}>")
target_link_directories(blobfish PRIVATE "$<$<CONFIG:RELEASE>:${BF_RELEASE_LIBDIR}>")
target_compile_options(blobfish PRIVATE "$<$<CONFIG:DEBUG>:${BF_DEBUG_OPTIONS}>")
target_compile_options(blobfish PRIVATE "$<$<CONFIG:RELEASE>:${BF_RELEASE_OPTIONS}>")
target_link_libraries(blobfish "$<$<CONFIG:DEBUG>:${BF_DEBUG_LIBRARIES}>")
target_link_libraries(blobfish "$<$<CONFIG:RELEASE>:${BF_RELEASE_LIBRARIES}>")
