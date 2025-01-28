cmake_minimum_required(VERSION 3.10)
project(ConsoleGame)

# Set C++ standard to C++17, which is the closest to C++19 available
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(${PROJECT_NAME} src/main.cpp)
