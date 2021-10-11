//
// Created by YuhangQ on 2021/9/30.
//

#ifndef INVODB_VIRTUAL_STORAGE_H
#define INVODB_VIRTUAL_STORAGE_H

#include <map>
#include "page_manager.h"

class VirtualStorage {
public:
    static VirtualStorage& Instance() {
        static VirtualStorage instance;
        return instance;
    }
    static int loadDatabase(const char *filename);
    StoragePage getPage(const int &index);
    void setPage(const int &index, const StoragePage &page);
    int allocate();
private:
    struct TableEl {
        int addr;
        int next;
    };
    // 页表 Cache
    std::map<int, TableEl> table;
    int getPhysicalAddress(const int &index);
    int getNextAddress(const int &index);
    void loadPageInfoTable(const int &index);
    // 私有化实现单例
    VirtualStorage() {}
    ~VirtualStorage() {}
    VirtualStorage(const PageManager&);
    VirtualStorage& operator=(const PageManager&);
};

#endif //INVODB_VIRTUAL_STORAGE_H
