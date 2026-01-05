cmake_minimum_required(VERSION 4.0)
project(neon-kernel LANGUAGES CUDA CXX)

set(CMAKE_CUDA_ARCHITECTURES native)
set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    src/Main.cpp
    src/Config.cpp
)

set(INCLUDES
    src
    src/utils
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})
