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

        util/prelude.hpp
        util/named_type.hpp

        runtime/prelude.hpp
        runtime/runtime_defs.hpp
        runtime/runtime.hpp
        runtime/runtime.cpp
        runtime/runtime_config.hpp
        runtime/runtime_builder.hpp
        runtime/runtime_builder.cpp
        runtime/task/prelude.hpp
        runtime/task/task_defs.hpp
        runtime/task/task.hpp
        runtime/task/task.cpp
        runtime/task/spawn.hpp
        runtime/timer/timer.hpp
        runtime/timer/timer.cpp
        runtime/waker/waker.hpp
        runtime/waker/waker.cpp
)

target_include_directories(libblobfish PRIVATE .)

set_target_properties(libblobfish PROPERTIES PREFIX "")
target_compile_options(libblobfish PRIVATE "$<$<CONFIG:DEBUG>:${BF_DEBUG_OPTIONS}>")
target_compile_options(libblobfish PRIVATE "$<$<CONFIG:RELEASE>:${BF_RELEASE_OPTIONS}>")
target_link_libraries(libblobfish "$<$<CONFIG:DEBUG>:${BF_DEBUG_LIBRARIES}>")
target_link_libraries(libblobfish "$<$<CONFIG:RELEASE>:${BF_RELEASE_LIBRARIES}>")
