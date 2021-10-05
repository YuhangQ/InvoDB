//
// Created by YuhangQ on 2021/9/24.
//

#include "page_manager.h"

int PageManager::loadDatabase(const char *filename) {
    stream.open(filename);

    printf("%d", stream.is_open());

    stream << "hello" << std::endl;
    stream.flush();
    return 0;
}

StoragePage& PageManager::getPage(const int &index) {
    StoragePage page;
    // 调整指针位置
    stream.seekg((index - 1) << 2);
    stream.read(page, 4096);
    return page;
}