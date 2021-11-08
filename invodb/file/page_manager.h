//
// Created by YuhangQ on 2021/9/24.
//

#ifndef INVODB_PAGE_MANAGER_H
#define INVODB_PAGE_MANAGER_H

#include <iostream>
#include <fstream>
#include <map>

#include "storage_page.h"
#include "json/json.hpp"
#include "invodb/utils/cache.h"

template<typename T, int T_SIZE>
class List;

class PageManager {
public:
    static PageManager& Instance() {
        static PageManager instance;
        return instance;
    }
    static int loadDatabase(const char *filename);

    StoragePage getPage(const int &index);
    void setPage(const int &index, const StoragePage &page);
    template<typename T>
    static std::shared_ptr<T> getNode(const int &index);
    int allocate();
    void release(const int &index, const bool &next = true);
    int saveJSONToFile(const nlohmann::json& json);
    nlohmann::json readJSONFromFile(const int &index);
private:
    std::map<int, StoragePage> map;
    std::fstream stream;
    LRUCache<int, std::shared_ptr<StoragePage>> cache;
    // 私有化实现单例
    PageManager():cache(LRUCache<int, std::shared_ptr<StoragePage>>(100000)) {}
    ~PageManager() {}
    PageManager(const PageManager&);
    PageManager& operator=(const PageManager&);
};


#endif //INVODB_PAGE_MANAGER_H
