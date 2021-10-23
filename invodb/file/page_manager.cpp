//
// Created by YuhangQ on 2021/9/24.
//

#include "page_manager.h"

int PageManager::loadDatabase(const char *filename) {
    Instance().stream.open(filename);
    return 0;
}

StoragePage PageManager::getPage(const int &index) {
    StoragePage page(index);
    // 调整指针位置
    stream.clear();
    stream.seekg(index * 1024);
    stream.read(page, 1024);
    return page;
}

void PageManager::setPage(const int &index, const StoragePage &page) {
    stream.clear();
    stream.seekg(index * 1024);
    stream.write(page, 1024);
}

int PageManager::allocate() {
    stream.seekp(0, std::ios::end);
    int index = stream.tellp() / 1024;
    setPage(index, StoragePage(index));
    return index;
}
