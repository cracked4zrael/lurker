cmake_minimum_required(VERSION 3.10)
project(lurker)

set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/includes)

add_executable(main
    main.cpp
    evasions/sandbox_evasions.cpp
)

target_link_libraries(main
    ws2_32
)

