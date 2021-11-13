//
// Created by YuhangQ on 2021/9/24.
//

#include "page_manager.h"

std::map<int, StoragePage> PageManager::map;
std::fstream PageManager::stream;

int PageManager::loadDatabase(const char *filename) { 
    Instance().stream.open(filename);
    Instance().stream.seekp(0, std::ios::end);
    if(!Instance().stream.is_open()) {
        std::ofstream file(filename, std::fstream::out);
        file.close();
        Instance().stream.open(filename);
        Instance().stream.seekp(0, std::ios::end);
    }
    int index = Instance().stream.tellp() / 1024;
    if(index == 0) {
        StoragePage page(0);
        page.setIntStartFrom(4, 2);
        page.save();
        StoragePage(1).save();
    }
    return 0;
}

StoragePage PageManager::getPage(const int &index) {

//    if(cache.exist(index)) {
//        return *cache.get(index);
//    }

    StoragePage page(index);
    // 调整指针位置
    stream.clear();
    stream.seekg((long long)index * 1024);
    stream.read(page, 1024);
    return page;
}

void PageManager::setPage(const int &index, const StoragePage &page) {

//    auto p = cache.put(index, std::make_shared<StoragePage>(page));
//    if(p == nullptr) return;

    stream.clear();
    stream.seekg((long long)index * 1024);
    stream.write(page, 1024);
}

int PageManager::allocate() {

//     try to allocate from free block list
    auto page = getPage(0);
    int index =  page.getIntStartFrom(0);
    if(index != 0) {
        auto allocatePage = getPage(index);

        page.setIntStartFrom(0, allocatePage.next());
        // reset
        allocatePage.clear();
        allocatePage.save();
        page.save();
        //printf("1allocate: %d\n", index);
        return index;
    }

    index = page.getIntStartFrom(4);
    //printf("2allocate: %d\n", index);

    page.setIntStartFrom(4, index + 1);
    page.save();
    //      allocate block at the end
//    stream.seekp(0, std::ios::end);
//    index = stream.tellp() / 1024;
    setPage(index, StoragePage(index));
    return index;
}

void PageManager::release(const int &index, const bool &next) {
    auto page = getPage(0);
    int head = page.getIntStartFrom(0);
    auto releasePage = getPage(index);
    releasePage.setNext(head);
    page.setIntStartFrom(0, releasePage.getAddress());
    page.save();
    releasePage.save();
    //printf("release: %d\n", index);

}

nlohmann::json PageManager::readJSONFromFile(const int &index) {
    std::string content;

    auto page = getPage(index);
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

    auto page = getPage(allocate());
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


