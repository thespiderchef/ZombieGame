cmake_minimum_required(VERSION 3.10)
project(ZombieGame VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    ZombieGame/main.cpp
)

add_executable(ZombieGame ${SOURCES})
target_include_directories(ZombieGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ZombieGame)

install(TARGETS ZombieGame RUNTIME DESTINATION bin)
