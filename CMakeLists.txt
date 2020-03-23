cmake_minimum_required(VERSION 3.15)
project(tz)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-Wall -Wextra -Wnon-virtual-dtor -Werror)

add_executable(${CMAKE_PROJECT_NAME} main.cpp Matcher.cpp Batcher.cpp WorkerPool.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} pthread)
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -pthread)