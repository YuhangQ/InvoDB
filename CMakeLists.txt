cmake_minimum_required(VERSION 3.16.3)
project(InvoDB)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

include_directories(.)
include_directories(./invodb)

add_executable(InvoDB
        invodb/main.cpp
        invodb/main.h invodb/file/page_manager.cpp invodb/file/page_manager.h invodb/models/collection.cpp invodb/models/collection.h invodb/file/storage_page.cpp invodb/file/storage_page.h invodb/utils/logger.h invodb/utils/uuid.h invodb/btree/node.h invodb/btree/btree.h)
