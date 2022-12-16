cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

project(libblobfish)

set(BF_DEBUG_OPTIONS -ggdb -O0 -Wall -Wextra -pedantic -march=native -fsanitize=address -fno-omit-frame-pointer -pthread)
set(BF_RELEASE_OPTIONS -O3 -Wall -Wextra -pedantic -march=native -pthread)
set(BF_DEBUG_LIBRARIES -lasan -lmagic)
set(BF_RELEASE_LIBRARIES -lmagic)

add_library(libblobfish

        blobfish.hpp
        blobfish.cpp
)

set_target_properties(libblobfish PROPERTIES PREFIX "")
target_compile_options(libblobfish PRIVATE "$<$<CONFIG:DEBUG>:${BF_DEBUG_OPTIONS}>")
target_compile_options(libblobfish PRIVATE "$<$<CONFIG:RELEASE>:${BF_RELEASE_OPTIONS}>")
target_link_libraries(libblobfish "$<$<CONFIG:DEBUG>:${BF_DEBUG_LIBRARIES}>")
target_link_libraries(libblobfish "$<$<CONFIG:RELEASE>:${BF_RELEASE_LIBRARIES}>")