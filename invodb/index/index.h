//
// Created by YuhangQ on 2021/11/1.
//

#ifndef INVODB_INDEX_H
#define INVODB_INDEX_H

#include "btree/btree.h"
#include "json/json.hpp"

class Index {
public:
    void insert(const nlohmann::json& json);
private:
    template<typename T>
    void insertElement(T const& key, const int& add) {
        BTree<T, 128> treeString;
        BTree<T, 1> treeBool;
        BTree<T, 8> treeDouble;
        BTree<T, 4> treeInt;
    }
};


#endif //INVODB_INDEX_H
