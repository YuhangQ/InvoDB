//
// Created by YuhangQ on 2021/9/24.
//

#include "page_manager.h"
#include "btree/list.h"

List<int, 4>* PageManager::freeList = new List<int, 4>(1);

int PageManager::loadDatabase(const char *filename) {
    Instance().stream.open(filename);
    Instance().stream.seekp(0, std::ios::end);
    int index = Instance().stream.tellp() / 1024;
    if(index == 0) {
        StoragePage(0).save();
        StoragePage(1).save();
        StoragePage(2).save();
    }
    return 0;
}

StoragePage PageManager::getPage(const int &index) {

    /*
    if(cache.exist(index)) {
        return cache.get(index);
    }
     */

    StoragePage page(index);
    // 调整指针位置
    stream.clear();
    stream.seekg(index * 1024);
    stream.read(page, 1024);
    return page;
}

void PageManager::setPage(const int &index, const StoragePage &page) {
    //cache.put(index, page);
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

void PageManager::release(const int &index, const bool &next) {

    return;
    auto page = getPage(index);
    freeList->insert(page.getAddress());
    if(next) {
        while(page.next()) {
            freeList->insert(page.next());
            page = getPage(page.next());
        }
    }
}

nlohmann::json PageManager::readJSONFromFile(const int &index) {
    std::string content;

    StoragePage page = getPage(index);
    while(true) {
        for(int i=0; i<1016; i++) {
            if(page[i] == '\0') break;
            content.push_back(page[i]);
        }
        if(page.next() == 0) break;
        page = getPage(page.next());
    }

    return nlohmann::json::parse(content);
}

int PageManager::saveJSONToFile(const nlohmann::json& json) {
    std::string content = json.dump();
    int size = content.size();

    StoragePage page = getPage(allocate());
    int res = page.getAddress();
    int p = 0;
    while(p < size) {
        int len = std::min(size - p, 1016);
        page.setStartFrom(0, &content.c_str()[p], len);
        page.save();
        p += len;
        if(p < size) {
            int newPage = allocate();
            int lastPage = page.getAddress();
            page.setNext(newPage);
            page.save();
            page = getPage(newPage);
            page.setLast(lastPage);
            page.save();
        }
    }
    return res;
}


