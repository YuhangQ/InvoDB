cmake_minimum_required(VERSION 3.20)
project(InvoDB)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(InvoDB
        invodb/main.cpp
        invodb/main.h invodb/file/page_manager.cpp invodb/file/page_manager.h invodb/file/virtual_storage.cpp invodb/file/virtual_storage.h invodb/models/json.cpp invodb/models/json.h invodb/invodb.cpp invodb/invodb.h invodb/models/collection.cpp invodb/models/collection.h)
