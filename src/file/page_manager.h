//
// Created by YuhangQ on 2021/9/24.
//

#ifndef INVODB_PAGE_MANAGER_H
#define INVODB_PAGE_MANAGER_H

#include <iostream>
#include <fstream>
#include <map>

#include "../json/json.hpp"
#include "../utils/cache.h"
#include "storage_page.h"

class PageManager {
public:
    static PageManager& Instance() {
        static PageManager* instance;
        if(instance == nullptr) instance = new PageManager();
        return *instance;
    }
    static int loadDatabase(const char *filename);

    static StoragePage getPage(const int &index);
    static void setPage(const int &index, const StoragePage &page);
    template<typename T>
    static std::shared_ptr<T> getNode(const int &index);
    static int allocate();
    static void release(const int &index, const bool &next = true);
    static int saveJSONToFile(const nlohmann::json& json);
    static nlohmann::json readJSONFromFile(const int &index);
    static std::map<int, StoragePage> map;
    static std::fstream stream;
    // 私有化实现单例
    PageManager() {}
    ~PageManager() {}
    PageManager(const PageManager&);
    PageManager& operator=(const PageManager&);
};


#endif //INVODB_PAGE_MANAGER_H
