//
// Created by YuhangQ on 2021/10/9.
//

#include "collection.h"

Collection::Collection(const std::string &name, const int &firstPage) {
    Logger::info<std::string, std::string>("load Collection: ", name);
}

void Collection::insert(JSON &json) {
    if(!json.HasMember("__Invo_ID__")) {
        Document::AllocatorType &allocator = json.GetAllocator();
        Value invoid (kStringType);
        invoid.SetString(generateUUID().c_str(), 32);
        json.AddMember("__Invo_ID__", invoid, allocator);
    }
    Logger::info<std::string, std::string>("INSERT ", json.ToString());
}


std::map<std::string, Collection*> Collection::map;
std::set<int> Collection::free;

void Collection::loadCollections() {
    // 前四页为集合信息页
    for (int id = 0; id < 4; id++) {
        StoragePage page = PageManager::Instance().getPage(id);
        PageManager::Instance().setPage(id, page);
        for (int i = 0; i < 32; i++) {
            int p = i * 32;
            int len = strlen(&page[p]);
            std::string name(&page[p], len > 28 ? 28 : len);
            int firstPage = page.getIntStartFrom(p + 28);
            // if free
            if (firstPage == 0) free.insert(id * 32 + i);
                // not free
            else map.insert(make_pair(name, new Collection(name, firstPage)));
        }
    }
    Logger::info<std::string, int>("Successfully load Collections: ", 128 - free.size());
}

Collection& Collection::createCollection(const std::string &name) {
    // exist
    if(map.count(name) != 0) {
        throw "collection has already exist";
    }
    // no free line
    if(free.size() == 0) {
        throw "you are reach the max limit count of collections";
    }
    int id = *free.begin();
    free.erase(free.begin());

    printf("id: %d\n", id / 32);
    StoragePage page = PageManager::Instance().getPage(id / 32);
    id %= 32;

    int collectionPage = PageManager::Instance().allocate();

    if(name.size() > 28) {
        throw "too long name of collection";
    }

    page.setStringStartFrom(id*32, name.c_str());
    page.setIntStartFrom(id*32+28, collectionPage);
    page.print();
    page.save();

    Collection *col = new Collection(name, collectionPage);

    map.insert(make_pair(name, col));

    return *col;
}

Collection &Collection::getCollection(const std::string &name) {
    if(map.count(name) == 0) {
        throw "no such collection";
    }
    return *map[name];
}

