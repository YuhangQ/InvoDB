cmake_minimum_required(VERSION 3.16.3)
project(InvoDB)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

include_directories(.)
include_directories(./invodb)

add_executable(InvoDB
        main.cpp
        file/page_manager.cpp file/page_manager.h collection/collection.cpp collection/collection.h file/storage_page.cpp file/storage_page.h utils/logger.h utils/uuid.h btree/node.h btree/btree.h btree/list.h collection/query.cpp utils/cache.h collection/index.cpp)