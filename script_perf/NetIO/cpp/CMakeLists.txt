
# CMake version
cmake_minimum_required (VERSION 2.6)

project(netio_cpp C CXX)

enable_testing()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

# Show the compile process
set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
    add_compile_options(-pthread)
endif()
if(CMAKE_THREAD_LIBS_INIT)
    link_libraries(${CMAKE_THREAD_LIBS_INIT})
endif()

set(CMAKE_CXX_FLAGS "-g -Wall -std=gnu++11")
set(CMAKE_CXX_COMPILER "clang++")

aux_source_directory(. DIR_SRCS)
add_executable(netio_cpp ${DIR_SRCS})
