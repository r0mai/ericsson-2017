cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost COMPONENTS program_options)
find_package(CapnProto REQUIRED)
find_package(SFML 2 REQUIRED system window graphics network)

include_directories(${SFML_INCLUDE_DIR})
include_directories(${CAPNP_INCLUDE_DIRS})
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

add_library(xonixLib
    src/gui.cpp
    src/Connection.cpp
    src/Client.cpp
    src/Model.cpp
    src/Server.cpp
    src/Direction.cpp
    src/DumbPlayer.cpp
    src/Utils.cpp
    src/protocol/Common.capnp.c++
    src/protocol/Command.capnp.c++
    src/protocol/Response.capnp.c++
)

add_executable(xonix src/client_main.cpp)
add_executable(server src/server_main.cpp)

target_link_libraries(xonix xonixLib ${SFML_LIBRARIES} ${CAPNP_LIBRARIES} ${Boost_LIBRARIES})
target_link_libraries(server xonixLib ${SFML_LIBRARIES} ${CAPNP_LIBRARIES} ${Boost_LIBRARIES})
