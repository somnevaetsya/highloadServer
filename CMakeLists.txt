cmake_minimum_required(VERSION 3.16)
project(highload_srv)

find_package(nlohmann_json 3.10.5 REQUIRED)

set(CMAKE_CXX_STANDARD 20)
include_directories(include)
add_executable(highload_srv
        main.cpp
        include/server.h
        src/server.cpp
        src/handler.cpp
        include/handler.h
        src/conf.cpp include/conf.h)

target_link_libraries(highload_srv -lpthread nlohmann_json::nlohmann_json)
