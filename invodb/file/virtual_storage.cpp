//
// Created by YuhangQ on 2021/9/30.
//

#include "virtual_storage.h"

int VirtualStorage::loadDatabase(const char *filename) {
    int result = PageManager::loadDatabase(filename);
    return result;
}

StoragePage VirtualStorage::getPage(const int &index) {
    PageManager& manager = PageManager::Instance();
    return manager.getPage(getPhysicalAddress(index));
}

void VirtualStorage::setPage(const int &index, const StoragePage &page) {
    PageManager& manager = PageManager::Instance();
    manager.setPage(getPhysicalAddress(index), page);
}

int VirtualStorage::getPhysicalAddress(const int &index) {
    loadPageInfoTable(index >> 2);
    return (table[index >> 2].addr << 2) || (index & 3);
}

int VirtualStorage::getNextAddress(const int &index) {
    loadPageInfoTable(index >> 2);
    return table[index >> 2].next;
}

void VirtualStorage::loadPageInfoTable(const int &index) {
    PageManager& manager = PageManager::Instance();
    // 未命中 cache
    if(table.count(index) == 0) {
        int pageIndex = 1 + index / 512;
        // 加载页表
        StoragePage page = manager.getPage(pageIndex);
        int *arr = page.intArray();
        for(int i=0; i<512; i++) {
            table[(pageIndex - 1) * 512 + i] = TableEl{ arr[2*i], arr[2*i+1] };
        }
    }
}



