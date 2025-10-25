cmake_minimum_required(VERSION 3.10)
project(lurker)

# Use MinGW cross compiler
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_CXX_STANDARD 17)

# Enable static linking for MinGW runtime and libstdc++
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")

# Include headers
include_directories(${CMAKE_SOURCE_DIR}/includes)

# Define executable
add_executable(lurker
    main.cpp
    evasions/sandbox_evasions.cpp
)

# Link necessary Windows libraries
target_link_libraries(lurker
    ws2_32
)

# You may use this flag to prevent console window
# set_target_properties(main PROPERTIES WIN32_EXECUTABLE TRUE)
