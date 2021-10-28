//
// Created by YuhangQ on 2021/9/24.
//

#ifndef INVODB_PAGE_MANAGER_H
#define INVODB_PAGE_MANAGER_H

#include <iostream>
#include <fstream>
#include <map>

#include "storage_page.h"

class PageManager {
public:
    static PageManager& Instance() {
        static PageManager instance;
        return instance;
    }
    static int loadDatabase(const char *filename);
    StoragePage getPage(const int &index);
    void setPage(const int &index, const StoragePage &page);
    int allocate();
    void free(const int &index);
private:
    std::map<int, StoragePage> map;
    std::fstream stream;
    // 私有化实现单例
    PageManager() {}
    ~PageManager() {}
    PageManager(const PageManager&);
    PageManager& operator=(const PageManager&);
};


#endif //INVODB_PAGE_MANAGER_H
