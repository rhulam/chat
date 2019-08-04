cmake_minimum_required(VERSION 3.14)
project(chat)

set(CMAKE_CXX_STANDARD 14)
set( CMAKE_CXX_FLAGS "-Wall -Wextra -Werror" )
find_package (Threads)
ADD_LIBRARY(sharedLib STATIC shared/shared.cpp)

add_executable(server server.cpp)
add_executable(client client.cpp)

target_link_libraries(server ${CMAKE_THREAD_LIBS_INIT} sharedLib)
target_link_libraries(client ${CMAKE_THREAD_LIBS_INIT} sharedLib)