cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

add_executable(xonix
    main.cpp
    protocol/Common.capnp.c++
    protocol/Command.capnp.c++
    protocol/Response.capnp.c++
)

