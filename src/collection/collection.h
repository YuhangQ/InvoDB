//
// Created by YuhangQ on 2021/10/9.
//

#ifndef INVODB_COLLECTION_H
#define INVODB_COLLECTION_H

#include "../file/page_manager.h"
#include "../utils/logger.h"
#include "../btree/btree.h"
#include "../json/json.hpp"
#include <map>
#include <set>
#include <algorithm>
#include <cstring>
#include "../utils/uuid.h"
#include "../btree/list.h"

class Collection {
public:
    void insert(nlohmann::json &json);
    void remove(const nlohmann::json &json);
    std::vector<nlohmann::json> query(const nlohmann::json &json);
    static void loadCollections();
    static Collection& getCollection(const std::string& name);
    static bool existsCollection(const std::string& name);
    static Collection& createCollection(const std::string& name);

    void test();

private:
    void indexJSON(const std::string prefix, const nlohmann::json &json, const int& address);
    void insertIndex(const std::string indexName, const std::string indexValue, const int& address);
    void insertIndex(const std::string indexName, double indexValue, const int& address);
    void insertIndex(const std::string indexName, bool indexValue, const int& address);
    void clearIndex(const std::string prefix, const nlohmann::json &json, const int& address);
    void removeIndex(const std::string indexName, const std::string indexValue, const int& address);
    void removeIndex(const std::string indexName, double indexValue, const int& address);
    void removeIndex(const std::string indexName, bool indexValue, const int& address);

    std::set<nlohmann::json> setIntersection(const std::set<nlohmann::json> &a, const std::set<nlohmann::json> &b);
    std::set<nlohmann::json> setUnion(const std::set<nlohmann::json> &a, const std::set<nlohmann::json> &b);

    std::set<nlohmann::json> innerQuery(const std::string &prefix, const nlohmann::json &json);
    std::set<nlohmann::json> queryString(const std::string &prefix, const std::string &minValue, const std::string &maxValue, const int &mod = 0);
    std::set<nlohmann::json> queryNumber(const std::string &prefix, const double &minValue, const double &maxValue, const int &mod = 0);
    std::set<nlohmann::json> queryBool(const std::string &prefix, const bool &value);
    std::set<nlohmann::json> queryRange(const std::string &prefix, const nlohmann::json &json);

    static std::map<std::string, Collection*> map;
    static BTree<std::string, 32> colList;

    BTree<std::string, 32> *uuid;
    BTree<std::string, 128> *index;

    Collection(const std::string& name,const int& firstPage);
    Collection() {}
    ~Collection() {}
    Collection(const Collection&);
    Collection& operator=(const Collection&);
};


#endif //INVODB_COLLECTION_H
