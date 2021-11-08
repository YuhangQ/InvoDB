//
// Created by YuhangQ on 2021/10/9.
//

#include "collection.h"

BTree<std::string, 32> Collection::colList(1);
std::map<std::string, Collection*> Collection::map;

void Collection::loadCollections() {
    int cnt = 0;
    for(auto& key : colList.keySet()) {
        map.insert(std::make_pair(key, new Collection(key, colList.find(key))));
        cnt++;
    }
    Logger::info<std::string, int>("Successfully load Collections: ", cnt);
}

Collection& Collection::createCollection(const std::string &name) {
    // exist
    if(map.count(name) != 0) {
        return *map[name];
    }
    colList.insert(name, PageManager::Instance().allocate());
    Collection *col = new Collection(name, colList.find(name));
    map.insert(make_pair(name, col));
    return *col;
}

Collection &Collection::getCollection(const std::string &name) {
    if(map.count(name) == 0) {
        throw "no such collection";
    }
    return *map[name];
}

Collection::Collection(const std::string &name, const int &firstPage) {
    Logger::info<std::string, std::string>("load Collection: ", name);
    index = new BTree<std::string, 128>(firstPage);
    if(!index->exists("__INVO_ID__")) {
        index->insert("__INVO_ID__", PageManager::Instance().allocate());
    }
    uuid = new BTree<std::string, 32>(index->find("__INVO_ID__"));
}

void Collection::insert(nlohmann::json &json) {

    if(json["__INVO_ID__"].empty()) {
        json["__INVO_ID__"] = generateUUID();
    } else {
        remove(json);
    }

    std::string id = json["__INVO_ID__"].get<std::string>();
    int add = PageManager::Instance().saveJSONToFile(json);
    uuid->insert(id, add);

    //Logger::info<std::string, std::string>("INSERT ", json.dump());

    // add index
    indexJSON("", json, add);
}

void Collection::remove(const nlohmann::json &json) {
    if(json["__INVO_ID__"].empty()) {
        throw "no invo_id";
    }
    std::string id = json["__INVO_ID__"].get<std::string>();


    int address = uuid->find(id);
    uuid->remove(id);

    nlohmann::json jsonInDisk = PageManager::Instance().readJSONFromFile(address);

    clearIndex("", json, address);

    PageManager::Instance().release(address);
}


void Collection::test() {
    index->print();
    auto qq = new BTree<std::string, 128>(7);
    while(true) {
        std::string q;
        std::cin >> q;
        List<int, 4> list(qq->find(q));
        //list.print();
        for(auto& add : list.all()) {
            std::cout << ">> " << PageManager::Instance().readJSONFromFile(add).dump() << std::endl;
        }
    }
}
