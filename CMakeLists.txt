cmake_minimum_required(VERSION 3.16)
project(KnifeHit)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL3
find_package(SDL3 REQUIRED)

# Add source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Knife.cpp
    src/Target.cpp
    src/Renderer.cpp
)

# Add header files
set(HEADERS
    include/Game.hpp
    include/Knife.hpp
    include/Target.hpp
    include/Renderer.hpp
)

# Create executable
add_executable(KnifeHit ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(KnifeHit PRIVATE include)

# Link SDL3
target_link_libraries(KnifeHit PRIVATE SDL3::SDL3) 