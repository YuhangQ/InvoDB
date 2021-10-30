//
// Created by YuhangQ on 2021/10/9.
//

#ifndef INVODB_COLLECTION_H
#define INVODB_COLLECTION_H

#include "file/page_manager.h"
#include "utils/logger.h"
#include "btree/btree.h"
#include "models/json.h"
#include <map>
#include <set>
#include <algorithm>
#include <cstring>
#include "utils/uuid.h"

class Collection {
public:
    static void insert(JSON &json);
    static void loadCollections();
    static Collection& getCollection(const std::string& name);
    static Collection& createCollection(const std::string& name);
private:
    static std::map<std::string, Collection*> map;
    static std::set<int> free;

    BTree<27, std::string, 32> *tree;

    Collection(const std::string& name,const int& firstPage);
    Collection() {}
    ~Collection() {}
    Collection(const Collection&);
    Collection& operator=(const Collection&);
};


#endif //INVODB_COLLECTION_H
