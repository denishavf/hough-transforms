cmake_minimum_required(VERSION 3.18)
project(hough-transforms)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Include directory for header files
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add the source files to the project
add_executable(${CMAKE_PROJECT_NAME} 
    src/main.cpp
    src/matrix_filter.cpp
    src/stb_image.cpp)

# Include directory for third party
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third-party/stb)
