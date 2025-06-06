cmake_minimum_required(VERSION 3.16)
project(KnifeHit)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL3
find_package(SDL3 REQUIRED)

# Find SDL3_ttf - ADD THIS LINE
find_package(SDL3_ttf REQUIRED)

# Add source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Knife.cpp
    src/Target.cpp
    src/Renderer.cpp
    src/FontManager.cpp  # ADD THIS LINE
    src/GameConstants.cpp
)

# Add header files
set(HEADERS
    include/Game.hpp
    include/Knife.hpp
    include/Target.hpp
    include/Renderer.hpp
    include/GameConstants.hpp
    include/FontManager.hpp  # ADD THIS LINE
)

# Create executable
add_executable(KnifeHit ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(KnifeHit PRIVATE include)

# Link SDL3 and SDL3_ttf - MODIFY THIS LINE
target_link_libraries(KnifeHit PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)