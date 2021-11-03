//
// Created by YuhangQ on 2021/10/9.
//

#include "collection.h"

BTree<std::string, 32> Collection::colList(2);
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
    Logger::info<std::string, std::string>("INSERT ", json.dump());

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

// age=1  age=“hello”
void Collection::indexJSON(const std::string prefix, const nlohmann::json &json, const int& address) {
    // even easier with structured bindings (C++17)
    for (auto& [key, value] : json.items()) {
        std::cout << prefix << key << " : " << value << "\n";
        if(value.is_string()) insertIndex(prefix + key, value.get<std::string>(), address);
        if(value.is_number()) insertIndex(prefix + key, value.get<double>(), address);
        if(value.is_boolean()) insertIndex(prefix + key, value.get<bool>(), address);
        if(value.is_object()) indexJSON(prefix + key + ".", value.get<nlohmann::json>(),address);
        if(value.is_array()) {
            for(auto& element : value.get<nlohmann::json>()) {
                if(element.is_string()) insertIndex(prefix + key, element.get<std::string>(), address);
                if(element.is_number()) insertIndex(prefix + key, element.get<double>(), address);
                if(element.is_boolean()) insertIndex(prefix + key, element.get<bool>(), address);
            }
        }
    }
}

void Collection::insertIndex(const std::string indexName, const std::string indexValue, const int &address) {
    printf("INDEX: %s = \"%s\" add:(%d)\n", indexName.c_str(), indexValue.c_str(), address);

    std::string treeName = indexName + "$string";
    if(!index->exists(treeName)) {
        index->insert(treeName, PageManager::Instance().allocate());
    }

    BTree<std::string, 128> indexTree(index->find(treeName));

    if(!indexTree.exists(indexValue)) {
        indexTree.insert(indexValue, PageManager::Instance().allocate());
    }

    List<int, 4> list(indexTree.find(indexValue));
    list.insert(address);
}

void Collection::insertIndex(const std::string indexName, double indexValue, const int &address) {
    printf("INDEX: %s = %f add:(%d)\n", indexName.c_str(), indexValue, address);

    std::string treeName = indexName + "$number";
    if(!index->exists(treeName)) {
        index->insert(treeName, PageManager::Instance().allocate());
    }

    BTree<double, 8> indexTree(index->find(treeName));

    if(!indexTree.exists(indexValue)) {
        indexTree.insert(indexValue, PageManager::Instance().allocate());
    }

    List<int, 4> list(indexTree.find(indexValue));
    list.insert(address);
}

void Collection::insertIndex(const std::string indexName, bool indexValue, const int &address) {
    printf("INDEX: %s = %s add:(%d)\n", indexName.c_str(), indexValue ? "true" : "false", address);

    std::string treeName = indexName + "$boolean";
    if(!index->exists(treeName)) {
        index->insert(treeName, PageManager::Instance().allocate());
    }

    BTree<bool, 1> indexTree(index->find(treeName));

    if(!indexTree.exists(indexValue)) {
        indexTree.insert(indexValue, PageManager::Instance().allocate());
    }

    List<int, 4> list(indexTree.find(indexValue));
    list.insert(address);
}

void Collection::clearIndex(const std::string prefix, const nlohmann::json &json, const int &address) {
    for (auto& [key, value] : json.items()) {
        std::cout << prefix << key << " : " << value << "\n";
        if(value.is_string()) removeIndex(prefix + key, value.get<std::string>(), address);
        if(value.is_number()) removeIndex(prefix + key, value.get<double>(), address);
        if(value.is_boolean()) removeIndex(prefix + key, value.get<bool>(), address);
        if(value.is_object()) clearIndex(prefix + key + ".", value.get<nlohmann::json>(),address);
        if(value.is_array()) {
            for(auto& element : value.get<nlohmann::json>()) {
                if(element.is_string()) removeIndex(prefix + key, element.get<std::string>(), address);
                if(element.is_number()) removeIndex(prefix + key, element.get<double>(), address);
                if(element.is_boolean()) removeIndex(prefix + key, element.get<bool>(), address);
            }
        }
    }
}

void Collection::removeIndex(const std::string indexName, const std::string indexValue, const int &address) {
    printf("REMOVE: %s = \"%s\" add:(%d)\n", indexName.c_str(), indexValue.c_str(), address);

    std::string treeName = indexName + "$string";
    if(!index->exists(treeName)) {
        index->insert(treeName, PageManager::Instance().allocate());
    }

    BTree<std::string, 128> indexTree(index->find(treeName));

    if(!indexTree.exists(indexValue)) {
        indexTree.insert(indexValue, PageManager::Instance().allocate());
    }

    List<int, 4> list(indexTree.find(indexValue));
    list.remove(address);
}

void Collection::removeIndex(const std::string indexName, double indexValue, const int &address) {
    printf("REMOVE: %s = %f add:(%d)\n", indexName.c_str(), indexValue, address);

    std::string treeName = indexName + "$number";
    if(!index->exists(treeName)) {
        index->insert(treeName, PageManager::Instance().allocate());
    }

    BTree<double, 8> indexTree(index->find(treeName));

    if(!indexTree.exists(indexValue)) {
        indexTree.insert(indexValue, PageManager::Instance().allocate());
    }

    List<int, 4> list(indexTree.find(indexValue));
    list.remove(address);
}

void Collection::removeIndex(const std::string indexName, bool indexValue, const int &address) {
    printf("REMOVE: %s = %s add:(%d)\n", indexName.c_str(), indexValue ? "true" : "false", address);

    std::string treeName = indexName + "$boolean";
    if(!index->exists(treeName)) {
        index->insert(treeName, PageManager::Instance().allocate());
    }

    BTree<bool, 1> indexTree(index->find(treeName));

    if(!indexTree.exists(indexValue)) {
        indexTree.insert(indexValue, PageManager::Instance().allocate());
    }

    List<int, 4> list(indexTree.find(indexValue));
    list.remove(address);
}
