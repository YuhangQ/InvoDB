//
// Created by YuhangQ on 2021/11/7.
//

#include "collection.h"

// age=1  age=“hello”
void Collection::indexJSON(const std::string prefix, const nlohmann::json &json, const int& address) {
    // even easier with structured bindings (C++17)
    for (auto& [key, value] : json.items()) {
        //std::cout << prefix << key << " : " << value << "\n";
        if(key == "__INVO_ID__") continue;

        if(value.is_boolean()) insertIndex(prefix + key, value.get<bool>(), address);
        if(value.is_number()) insertIndex(prefix + key, value.get<double>(), address);
        if(value.is_string()) insertIndex(prefix + key, value.get<std::string>(), address);
        if(value.is_null()) insertNullIndex(prefix + key, address);
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

    std::string treeName = indexName + "$string";


    //printf("INDEX TO %s: %s = \"%s\" add:(%d)\n", treeName.c_str(), indexName.c_str(), indexValue.c_str(), address);

    if(!index->exists(treeName)) {
        index->insert(treeName, PageManager::allocate());
    }

    BTree<std::string, 64> indexTree(index->find(treeName));

    if(!indexTree.exists(indexValue)) {
        indexTree.insert(indexValue, PageManager::allocate());
    }

    List<int, 4> list(indexTree.find(indexValue));
    list.insert(address);
    //printf("INSERT %d INTO %d\n", address, indexTree.find(indexValue));
    //list.print();
}

void Collection::insertIndex(const std::string indexName, double indexValue, const int &address) {
    //printf("INDEX: %s = %f add:(%d)\n", indexName.c_str(), indexValue, address);

    std::string treeName = indexName + "$number";
    if(!index->exists(treeName)) {
        index->insert(treeName, PageManager::allocate());
    }

    BTree<double, 8> indexTree(index->find(treeName));

    if(!indexTree.exists(indexValue)) {
        indexTree.insert(indexValue, PageManager::allocate());
    }

    List<int, 4> list(indexTree.find(indexValue));
    list.insert(address);
}

void Collection::insertIndex(const std::string indexName, bool indexValue, const int &address) {
    //printf("INDEX: %s = %s add:(%d)\n", indexName.c_str(), indexValue ? "true" : "false", address);

    std::string treeName = indexName + "$boolean";
    if(!index->exists(treeName)) {
        index->insert(treeName, PageManager::allocate());
    }

    BTree<bool, 1> indexTree(index->find(treeName));

    if(!indexTree.exists(indexValue)) {
        indexTree.insert(indexValue, PageManager::allocate());
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
        if(value.is_null()) removeNullIndex(prefix + key, address);
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
    //printf("REMOVE: %s = \"%s\" add:(%d)\n", indexName.c_str(), indexValue.c_str(), address);

    std::string treeName = indexName + "$string";
    if(!index->exists(treeName)) {
        index->insert(treeName, PageManager::allocate());
    }

    BTree<std::string, 128> indexTree(index->find(treeName));

    if(!indexTree.exists(indexValue)) {
        indexTree.insert(indexValue, PageManager::allocate());
    }

    List<int, 4> list(indexTree.find(indexValue));
    list.remove(address);
}

void Collection::removeIndex(const std::string indexName, double indexValue, const int &address) {
    //printf("REMOVE: %s = %f add:(%d)\n", indexName.c_str(), indexValue, address);

    std::string treeName = indexName + "$number";
    if(!index->exists(treeName)) {
        index->insert(treeName, PageManager::allocate());
    }

    BTree<double, 8> indexTree(index->find(treeName));

    if(!indexTree.exists(indexValue)) {
        indexTree.insert(indexValue, PageManager::allocate());
    }

    List<int, 4> list(indexTree.find(indexValue));
    list.remove(address);
}

void Collection::removeIndex(const std::string indexName, bool indexValue, const int &address) {
    //printf("REMOVE: %s = %s add:(%d)\n", indexName.c_str(), indexValue ? "true" : "false", address);

    std::string treeName = indexName + "$boolean";
    if(!index->exists(treeName)) {
        index->insert(treeName, PageManager::allocate());
    }

    BTree<bool, 1> indexTree(index->find(treeName));

    if(!indexTree.exists(indexValue)) {
        indexTree.insert(indexValue, PageManager::allocate());
    }

    List<int, 4> list(indexTree.find(indexValue));
    list.remove(address);
}

void Collection::insertNullIndex(const std::string indexName, const int& address) {
    std::string treeName = indexName + "$null";
    if(!index->exists(treeName)) {
        index->insert(treeName, PageManager::allocate());
    }
    List<int, 4> list(index->find(treeName));
    list.insert(address);
}

void Collection::removeNullIndex(const std::string indexName, const int& address) {
    std::string treeName = indexName + "$null";
    if(!index->exists(treeName)) {
        index->insert(treeName, PageManager::allocate());
    }
    List<int, 4> list(index->find(treeName));
    list.remove(address);
}