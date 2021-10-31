//
// Created by YuhangQ on 2021/9/24.
//

#include "page_manager.h"

int PageManager::loadDatabase(const char *filename) {
    Instance().stream.open(filename);
    return 0;
}

StoragePage PageManager::getPage(const int &index) {
    StoragePage page(index);
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
    stream.flush();
}

int PageManager::allocate() {
    stream.seekp(0, std::ios::end);
    int index = stream.tellp() / 1024;
    setPage(index, StoragePage(index));
    return index;
}

void PageManager::release(const int &index) {

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


