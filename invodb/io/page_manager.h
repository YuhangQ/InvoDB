//
// Created by YuhangQ on 2021/9/24.
//

#ifndef INVODB_PAGE_MANAGER_H
#define INVODB_PAGE_MANAGER_H

#include <iostream>
#include <fstream>

class StoragePage {
public:
    char& operator[] (int index) { return this->page[index]; }
    operator const char *() { return this->page; }
    operator char *() { return this->page; }
    ~StoragePage() {
        printf("我被回收了");
    }
private:
    char page[4096];
};

class PageManager {
public:
    static PageManager& Instance() {
        static PageManager instance;
        return instance;
    }
    int loadDatabase(const char *filename);
    StoragePage& getPage(const int &index);
    void setPage(const int &index, const StoragePage &page);
private:
    std::fstream stream;

    // 私有化实现单例
    PageManager();
    ~PageManager();
    PageManager(const PageManager&);
    PageManager& operator=(const PageManager&);
};


#endif //INVODB_PAGE_MANAGER_H
