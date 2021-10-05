//
// Created by YuhangQ on 2021/9/24.
//

#include "page_manager.h"

int PageManager::loadDatabase(const char *filename) {
    stream.open(filename);
    stream.flush();
    return 0;
}

StoragePage& PageManager::getPage(const int &index) {

    StoragePage page;
    // 调整指针位置
    stream.clear();
    stream.seekg(index * 4096);
    stream.read(page, 4096);
    return page;
}

void PageManager::setPage(const int &index, const StoragePage &page) {
    stream.clear();
    stream.seekg(index * 4096);
    stream.write(page, 4096);
}

void StoragePage::print() {
    for(int i=0; i<16; i++) {
        for(int j=0; j<256; j++) {
            printf("%u ", page[i * 16 + j]);
        }
        printf("\n");
    }
}
