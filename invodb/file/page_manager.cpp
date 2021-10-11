//
// Created by YuhangQ on 2021/9/24.
//

#include "page_manager.h"

int PageManager::loadDatabase(const char *filename) {
    Instance().stream.open(filename);
    return 0;
}

StoragePage PageManager::getPage(const int &index) {
    StoragePage page;
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

void StoragePage::print() {
    for(int i=0; i<16; i++) {
        for(int j=0; j<256; j++) {
            printf("%u ", page[i * 16 + j]);
        }
        printf("\n");
    }
}

/**
 * 获取下一个连接的页，返回零则到达链结尾
 * @return
 */
int StoragePage::next() {
    return *((const int *)&page[1020]);
}

int StoragePage::setNext(const int &nextPage) {
    *((int *)&page[1020]) = nextPage;
    return 0;
}

int *StoragePage::intArray() {
    return (int *)page;
}

